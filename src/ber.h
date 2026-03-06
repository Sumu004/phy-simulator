#pragma once
#include <vector>

struct BERResult {
    double snr_db;
    int    total_bits;
    int    error_bits;
    double ber;
};

// Count bit errors between two equal-length bit vectors
int count_errors(const std::vector<int>& tx_bits, const std::vector<int>& rx_bits);

// Compute BER
BERResult compute_ber(double snr_db,
                      const std::vector<int>& tx_bits,
                      const std::vector<int>& rx_bits);
