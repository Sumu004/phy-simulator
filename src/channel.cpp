#include "channel.h"
#include <random>
#include <cmath>

AWGNChannel::AWGNChannel(double snr_db) : snr_db_(snr_db) {}

void AWGNChannel::set_snr(double snr_db) {
    snr_db_ = snr_db;
}

std::vector<Complex> AWGNChannel::transmit(const std::vector<Complex>& signal) const {
    double signal_power = 0.0;
    for (const auto& s : signal) signal_power += std::norm(s);
    signal_power /= signal.size();

    double snr_linear   = std::pow(10.0, snr_db_ / 10.0);
    double noise_sigma  = std::sqrt(signal_power / (2.0 * snr_linear));

    std::mt19937_64 rng(std::random_device{}());
    std::normal_distribution<double> noise_dist(0.0, noise_sigma);

    std::vector<Complex> received;
    received.reserve(signal.size());

    for (const auto& s : signal) {
        received.emplace_back(s.real() + noise_dist(rng),
                              s.imag() + noise_dist(rng));
    }
    return received;
}
