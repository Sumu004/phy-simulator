#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <iomanip>

#include "bit_generator.h"
#include "modulation/qpsk.h"
#include "modulation/qam16.h"
#include "ofdm_tx.h"
#include "ofdm_rx.h"
#include "channel.h"
#include "ber.h"

// ─── OFDM configuration ────────────────────────────────────────────────────
static const int NFFT       = 64;   // FFT size (64 subcarriers)
static const int CP_LEN     = 16;   // Cyclic prefix = 1/4 of NFFT
static const int N_DATA_SC  = 48;   // Data subcarriers (rest are pilots/guard)
static const int N_OFDM_SYM = 100;  // OFDM symbols per run

// ─── helpers ───────────────────────────────────────────────────────────────
void print_usage() {
    std::cout << "\nUsage:\n"
              << "  ./phy_simulator --mod <qpsk|qam16> --snr <dB>\n"
              << "  ./phy_simulator --mod <qpsk|qam16> --sweep          (SNR 0-20 dB)\n\n"
              << "Examples:\n"
              << "  ./phy_simulator --mod qpsk  --snr 10\n"
              << "  ./phy_simulator --mod qam16 --snr 15\n"
              << "  ./phy_simulator --mod qpsk  --sweep\n\n";
}

// ─── single simulation run at one SNR point ────────────────────────────────
BERResult run_simulation(const std::string& mod_scheme, double snr_db) {

    int bits_per_sym = (mod_scheme == "qpsk") ? QPSK::BITS_PER_SYMBOL
                                               : QAM16::BITS_PER_SYMBOL;
    int bits_per_ofdm = N_DATA_SC * bits_per_sym;
    int total_bits    = bits_per_ofdm * N_OFDM_SYM;

    // Ensure bits_per_ofdm divisible by bits_per_sym (it always will be here)
    std::vector<int> all_tx_bits = generate_bits(total_bits);
    std::vector<int> all_rx_bits;
    all_rx_bits.reserve(total_bits);

    OFDMTransmitter tx(NFFT, CP_LEN, N_DATA_SC);
    OFDMReceiver    rx(NFFT, CP_LEN, N_DATA_SC);
    AWGNChannel     channel(snr_db);

    for (int sym = 0; sym < N_OFDM_SYM; sym++) {
        // 1. Extract bits for this OFDM symbol
        std::vector<int> bits(all_tx_bits.begin() + sym * bits_per_ofdm,
                              all_tx_bits.begin() + (sym+1) * bits_per_ofdm);

        // 2. Modulate bits → complex symbols
        std::vector<Complex> mod_symbols;
        if (mod_scheme == "qpsk")
            mod_symbols = QPSK::modulate(bits);
        else
            mod_symbols = QAM16::modulate(bits);

        // 3. OFDM transmit (IFFT + cyclic prefix)
        std::vector<Complex> ofdm_signal = tx.transmit(mod_symbols);

        // 4. Channel (AWGN noise)
        std::vector<Complex> rx_signal = channel.transmit(ofdm_signal);

        // 5. OFDM receive (remove CP + FFT)
        std::vector<Complex> rx_symbols = rx.receive(rx_signal);

        // 6. Demodulate symbols → bits
        std::vector<int> rx_bits;
        if (mod_scheme == "qpsk")
            rx_bits = QPSK::demodulate(rx_symbols);
        else
            rx_bits = QAM16::demodulate(rx_symbols);

        // 7. Collect recovered bits
        for (int b : rx_bits) all_rx_bits.push_back(b);
    }

    return compute_ber(snr_db, all_tx_bits, all_rx_bits);
}

// ─── main ──────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {

    std::string mod_scheme = "qpsk";
    double      snr_db     = 10.0;
    bool        do_sweep   = false;

    // Parse CLI arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--mod" && i+1 < argc) {
            mod_scheme = argv[++i];
            if (mod_scheme != "qpsk" && mod_scheme != "qam16") {
                std::cerr << "Error: --mod must be 'qpsk' or 'qam16'\n";
                print_usage();
                return 1;
            }
        } else if (arg == "--snr" && i+1 < argc) {
            snr_db = std::stod(argv[++i]);
        } else if (arg == "--sweep") {
            do_sweep = true;
        } else if (arg == "--help" || arg == "-h") {
            print_usage();
            return 0;
        }
    }

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\n=== 5G PHY Simulator ===\n";
    std::cout << "Modulation : " << (mod_scheme == "qpsk" ? "QPSK" : "16-QAM") << "\n";
    std::cout << "OFDM       : " << NFFT << "-FFT, " << N_DATA_SC
              << " data subcarriers, CP=" << CP_LEN << "\n";
    std::cout << "OFDM Syms  : " << N_OFDM_SYM << "\n\n";

    if (do_sweep) {
        // ── SNR sweep 0 → 20 dB ──────────────────────────────────────────
        std::cout << std::left
                  << std::setw(12) << "SNR (dB)"
                  << std::setw(15) << "Total Bits"
                  << std::setw(12) << "Errors"
                  << std::setw(14) << "BER"
                  << "\n";
        std::cout << std::string(53, '-') << "\n";

        std::ofstream csv("results/ber_results.csv");
        csv << "snr_db,ber,modulation\n";

        for (double snr = 0.0; snr <= 20.0; snr += 2.0) {
            BERResult r = run_simulation(mod_scheme, snr);
            std::cout << std::setw(12) << snr
                      << std::setw(15) << r.total_bits
                      << std::setw(12) << r.error_bits
                      << std::setw(14) << r.ber
                      << "\n";
            csv << snr << "," << r.ber << "," << mod_scheme << "\n";
        }
        csv.close();
        std::cout << "\nResults saved to results/ber_results.csv\n";

    } else {
        // ── Single SNR point ─────────────────────────────────────────────
        std::cout << "SNR        : " << snr_db << " dB\n\n";
        BERResult r = run_simulation(mod_scheme, snr_db);

        std::cout << "Bits transmitted : " << r.total_bits  << "\n";
        std::cout << "Errors           : " << r.error_bits  << "\n";
        std::cout << "BER              : " << r.ber         << "\n\n";

        // Also write individual result to CSV for Python plotting
        std::ofstream csv("results/ber_results.csv");
        csv << "snr_db,ber,modulation\n";
        csv << r.snr_db << "," << r.ber << "," << mod_scheme << "\n";
        csv.close();
    }

    return 0;
}
