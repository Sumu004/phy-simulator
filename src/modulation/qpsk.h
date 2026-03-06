#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

// QPSK: 2 bits per symbol
// 00 → (+1+1i)/√2,  01 → (-1+1i)/√2
// 10 → (+1-1i)/√2,  11 → (-1-1i)/√2
class QPSK {
public:
    static const int BITS_PER_SYMBOL = 2;

    // Modulate: bits[] → complex symbols
    static std::vector<Complex> modulate(const std::vector<int>& bits);

    // Demodulate: complex symbols → bits[] (hard decision)
    static std::vector<int> demodulate(const std::vector<Complex>& symbols);

    // Constellation points (indexed 0-3 = 00,01,10,11)
    static Complex constellation(int index);
};
