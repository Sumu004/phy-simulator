#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class QPSK {
public:
    static const int BITS_PER_SYMBOL = 2;

    static std::vector<Complex> modulate(const std::vector<int>& bits);
    static std::vector<int> demodulate(const std::vector<Complex>& symbols);
    static Complex constellation(int index);
};
