#include "qpsk.h"
#include <cmath>
#include <stdexcept>

static const double INV_SQRT2 = 1.0 / std::sqrt(2.0);

// Constellation map: Gray-coded
// Index = 2-bit value (MSB first)
// 00=0 → (+1+1i)/√2
// 01=1 → (-1+1i)/√2
// 10=2 → (+1-1i)/√2
// 11=3 → (-1-1i)/√2
Complex QPSK::constellation(int index) {
    switch (index) {
        case 0: return Complex( INV_SQRT2,  INV_SQRT2); // 00
        case 1: return Complex(-INV_SQRT2,  INV_SQRT2); // 01
        case 2: return Complex( INV_SQRT2, -INV_SQRT2); // 10
        case 3: return Complex(-INV_SQRT2, -INV_SQRT2); // 11
        default: throw std::invalid_argument("QPSK index must be 0-3");
    }
}

std::vector<Complex> QPSK::modulate(const std::vector<int>& bits) {
    if (bits.size() % 2 != 0)
        throw std::invalid_argument("QPSK requires even number of bits");

    std::vector<Complex> symbols;
    symbols.reserve(bits.size() / 2);

    for (size_t i = 0; i + 1 < bits.size(); i += 2) {
        int index = (bits[i] << 1) | bits[i+1];
        symbols.push_back(constellation(index));
    }
    return symbols;
}

std::vector<int> QPSK::demodulate(const std::vector<Complex>& symbols) {
    std::vector<int> bits;
    bits.reserve(symbols.size() * 2);

    for (const auto& sym : symbols) {
        // Find nearest constellation point (minimum Euclidean distance)
        double best_dist = std::numeric_limits<double>::max();
        int best_idx = 0;
        for (int k = 0; k < 4; k++) {
            Complex diff = sym - constellation(k);
            double dist = std::norm(diff); // |diff|^2
            if (dist < best_dist) {
                best_dist = dist;
                best_idx = k;
            }
        }
        bits.push_back((best_idx >> 1) & 1);
        bits.push_back( best_idx       & 1);
    }
    return bits;
}
