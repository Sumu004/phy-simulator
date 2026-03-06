#include "ofdm_tx.h"
#include "fft.h"
#include <stdexcept>
#include <cmath>

OFDMTransmitter::OFDMTransmitter(int nfft, int cp_len, int n_data_sc)
    : nfft_(nfft), cp_len_(cp_len), n_data_sc_(n_data_sc)
{
    if ((nfft & (nfft - 1)) != 0)
        throw std::invalid_argument("OFDM nfft must be a power of 2");
    if (n_data_sc > nfft)
        throw std::invalid_argument("n_data_sc cannot exceed nfft");
}

std::vector<Complex> OFDMTransmitter::transmit(const std::vector<Complex>& symbols) const {
    if ((int)symbols.size() != n_data_sc_)
        throw std::invalid_argument("Symbol count must equal n_data_sc");

    // --- 1. Map symbols onto frequency-domain subcarriers ---
    // Place data subcarriers symmetrically around DC (null DC, null guard bands)
    CVector freq_domain(nfft_, Complex(0.0, 0.0));

    int half = n_data_sc_ / 2;
    for (int k = 0; k < half; k++) {
        freq_domain[k + 1]              = symbols[k];          // positive freqs
        freq_domain[nfft_ - half + k]   = symbols[half + k];   // negative freqs
    }

    // --- 2. IFFT → time domain ---
    ifft(freq_domain);

    // --- 3. Add cyclic prefix (copy last cp_len samples to front) ---
    std::vector<Complex> tx_signal;
    tx_signal.reserve(nfft_ + cp_len_);

    for (int i = nfft_ - cp_len_; i < nfft_; i++)
        tx_signal.push_back(freq_domain[i]);  // cyclic prefix
    for (int i = 0; i < nfft_; i++)
        tx_signal.push_back(freq_domain[i]);  // OFDM symbol

    return tx_signal;
}
