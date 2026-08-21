#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class OFDMReceiver {
public:
    OFDMReceiver(int nfft, int cp_len, int n_data_sc);

    std::vector<Complex> receive(const std::vector<Complex>& rx_signal) const;

private:
    int nfft_;
    int cp_len_;
    int n_data_sc_;
};
