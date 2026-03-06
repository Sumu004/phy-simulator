#pragma once
#include <vector>
#include <complex>
#include <cmath>

using Complex = std::complex<double>;
using CVector = std::vector<Complex>;

// Declarations only — definitions are in fft.cpp
void fft (CVector& a, bool inverse);
void ifft(CVector& a);
void fft (CVector& a);
