#include "fft.h"
#include <cmath>

// Cooley-Tukey radix-2 DIT FFT (in-place)
void fft(CVector& a, bool inverse) {
    int n = (int)a.size();
    // Bit-reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }
    // Butterfly stages
    for (int len = 2; len <= n; len <<= 1) {
        double angle = 2.0 * M_PI / len * (inverse ? 1.0 : -1.0);
        Complex wlen(std::cos(angle), std::sin(angle));
        for (int i = 0; i < n; i += len) {
            Complex w(1.0, 0.0);
            for (int j = 0; j < len / 2; j++) {
                Complex u = a[i + j];
                Complex v = a[i + j + len/2] * w;
                a[i + j]          = u + v;
                a[i + j + len/2]  = u - v;
                w *= wlen;
            }
        }
    }
    if (inverse) {
        for (auto& x : a) x /= static_cast<double>(n);
    }
}

void ifft(CVector& a) { fft(a, true);  }
void fft (CVector& a) { fft(a, false); }
