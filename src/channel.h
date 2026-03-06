#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

// Additive White Gaussian Noise (AWGN) channel model
// Adds complex Gaussian noise at a specified SNR
class AWGNChannel {
public:
    // snr_db: Signal-to-Noise Ratio in decibels
    explicit AWGNChannel(double snr_db);

    void set_snr(double snr_db);

    // Apply noise to signal, returns received signal
    std::vector<Complex> transmit(const std::vector<Complex>& signal) const;

    double get_snr_db()     const { return snr_db_; }
    double get_noise_var()  const { return noise_variance_; }

private:
    double snr_db_;
    double noise_variance_;   // σ² per complex sample
    void update_variance();
};
