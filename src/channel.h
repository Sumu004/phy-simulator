#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class AWGNChannel {
public:
    explicit AWGNChannel(double snr_db);

    void set_snr(double snr_db);

    std::vector<Complex> transmit(const std::vector<Complex>& signal) const;

private:
    double snr_db_;
};
