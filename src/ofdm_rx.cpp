#include "ofdm_rx.h"
#include "fft.h"
#include <stdexcept>

OFDMReceiver::OFDMReceiver(int nfft, int cp_len, int n_data_sc)
    : nfft_(nfft), cp_len_(cp_len), n_data_sc_(n_data_sc) {}

std::vector<Complex> OFDMReceiver::receive(const std::vector<Complex>& rx_signal) const {
    int expected_len = nfft_ + cp_len_;
    if ((int)rx_signal.size() != expected_len)
        throw std::invalid_argument("rx_signal length must equal nfft + cp_len");

    // --- 1. Remove cyclic prefix ---
    CVector time_domain(rx_signal.begin() + cp_len_, rx_signal.end());

    // --- 2. FFT → frequency domain ---
    fft(time_domain);

    // --- 3. Extract data subcarriers (mirror of TX mapping) ---
    // TX placed: positive freqs at [1 .. half], negative freqs at [nfft-half .. nfft-1]
    std::vector<Complex> symbols;
    symbols.reserve(n_data_sc_);

    int half = n_data_sc_ / 2;
    for (int k = 0; k < half; k++)
        symbols.push_back(time_domain[k + 1]);           // positive freqs

    for (int k = 0; k < half; k++)
        symbols.push_back(time_domain[nfft_ - half + k]); // negative freqs

    return symbols;
}
