#include "qam16.h"
#include <cmath>
#include <stdexcept>

// 16-QAM Gray-coded constellation
// I/Q axes each take values {-3, -1, +1, +3}
// Gray code for I: 00→-3, 01→-1, 11→+1, 10→+3
// Gray code for Q: same mapping
// Bit layout: [b0 b1 | b2 b3]  where b0b1 → I, b2b3 → Q

static int gray_to_level(int g) {
    // 2-bit gray → level index → {-3,-1,+1,+3}
    // Gray: 00→0, 01→1, 11→2, 10→3
    int bin = g ^ (g >> 1); // gray to binary
    static const int levels[4] = {-3, -1, 1, 3};
    return levels[bin & 3];
}

static int level_to_gray(int level) {
    // {-3,-1,+1,+3} → binary index → gray
    int idx;
    if      (level == -3) idx = 0;
    else if (level == -1) idx = 1;
    else if (level ==  1) idx = 2;
    else                  idx = 3;
    // binary to gray
    return idx ^ (idx >> 1);
}

Complex QAM16::constellation(int index) {
    if (index < 0 || index > 15)
        throw std::invalid_argument("16-QAM index must be 0-15");
    int I_gray = (index >> 2) & 3;
    int Q_gray =  index       & 3;
    double I = gray_to_level(I_gray) * SCALE;
    double Q = gray_to_level(Q_gray) * SCALE;
    return Complex(I, Q);
}

std::vector<Complex> QAM16::modulate(const std::vector<int>& bits) {
    if (bits.size() % 4 != 0)
        throw std::invalid_argument("16-QAM requires multiple of 4 bits");

    std::vector<Complex> symbols;
    symbols.reserve(bits.size() / 4);

    for (size_t i = 0; i + 3 < bits.size(); i += 4) {
        int index = (bits[i]   << 3) |
                    (bits[i+1] << 2) |
                    (bits[i+2] << 1) |
                     bits[i+3];
        symbols.push_back(constellation(index));
    }
    return symbols;
}

std::vector<int> QAM16::demodulate(const std::vector<Complex>& symbols) {
    std::vector<int> bits;
    bits.reserve(symbols.size() * 4);

    for (const auto& sym : symbols) {
        double best_dist = std::numeric_limits<double>::max();
        int best_idx = 0;
        for (int k = 0; k < 16; k++) {
            double dist = std::norm(sym - constellation(k));
            if (dist < best_dist) {
                best_dist = dist;
                best_idx = k;
            }
        }
        bits.push_back((best_idx >> 3) & 1);
        bits.push_back((best_idx >> 2) & 1);
        bits.push_back((best_idx >> 1) & 1);
        bits.push_back( best_idx       & 1);
    }
    return bits;
}
