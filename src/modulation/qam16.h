#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class QAM16 {
public:
    static const int BITS_PER_SYMBOL = 4;

    static std::vector<Complex> modulate(const std::vector<int>& bits);
    static std::vector<int>     demodulate(const std::vector<Complex>& symbols);
    static Complex              constellation(int index);

private:
    static constexpr double SCALE = 0.31622776601683794;
};
