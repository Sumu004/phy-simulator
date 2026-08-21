#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class OFDMTransmitter {
public:
    OFDMTransmitter(int nfft, int cp_len, int n_data_sc);

    std::vector<Complex> transmit(const std::vector<Complex>& symbols) const;

private:
    int nfft_;
    int cp_len_;
    int n_data_sc_;
};
