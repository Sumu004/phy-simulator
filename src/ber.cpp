#include "ber.h"
#include <stdexcept>

int count_errors(const std::vector<int>& tx, const std::vector<int>& rx) {
    if (tx.size() != rx.size())
        throw std::invalid_argument("Bit vectors must be equal length for BER");
    int errors = 0;
    for (size_t i = 0; i < tx.size(); i++)
        if (tx[i] != rx[i]) errors++;
    return errors;
}

BERResult compute_ber(double snr_db,
                      const std::vector<int>& tx_bits,
                      const std::vector<int>& rx_bits) {
    int errors = count_errors(tx_bits, rx_bits);
    int total  = (int)tx_bits.size();
    double ber = (total > 0) ? (double)errors / total : 0.0;
    return {snr_db, total, errors, ber};
}
