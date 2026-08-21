#pragma once
#include <vector>
#include <complex>
#include <cmath>

using Complex = std::complex<double>;
using CVector = std::vector<Complex>;

void fft (CVector& a, bool inverse);
void ifft(CVector& a);
void fft (CVector& a);
