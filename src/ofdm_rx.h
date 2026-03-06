#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class OFDMReceiver {
public:
    OFDMReceiver(int nfft, int cp_len, int n_data_sc);

    // Remove CP, FFT, extract data subcarriers
    // input: time-domain OFDM signal (length = nfft + cp_len)
    // returns: complex symbols on data subcarriers
    std::vector<Complex> receive(const std::vector<Complex>& rx_signal) const;

    int get_nfft()       const { return nfft_; }
    int get_cp_len()     const { return cp_len_; }
    int get_n_data_sc()  const { return n_data_sc_; }

private:
    int nfft_;
    int cp_len_;
    int n_data_sc_;
};
