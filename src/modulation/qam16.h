#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

// 16-QAM: 4 bits per symbol
// 4x4 square constellation, Gray-coded
// Normalised so average power = 1
class QAM16 {
public:
    static const int BITS_PER_SYMBOL = 4;

    static std::vector<Complex> modulate(const std::vector<int>& bits);
    static std::vector<int>     demodulate(const std::vector<Complex>& symbols);
    static Complex              constellation(int index);

private:
    // Normalization factor for unit average power
    // Average power of unnormalized 16-QAM = 10, so scale = 1/√10
    static constexpr double SCALE = 0.31622776601683794; // 1.0 / sqrt(10.0)
};
