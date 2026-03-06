#pragma once
#include <vector>
#include <complex>

using Complex = std::complex<double>;

class OFDMTransmitter {
public:
    // nfft        : FFT size (number of subcarriers), must be power of 2
    // cp_len      : cyclic prefix length (samples)
    // n_data_sc   : number of data-bearing subcarriers (<= nfft)
    OFDMTransmitter(int nfft, int cp_len, int n_data_sc);

    // Map modulated symbols onto subcarriers, run IFFT, add cyclic prefix
    // symbols.size() must equal n_data_sc
    // Returns time-domain OFDM signal (length = nfft + cp_len)
    std::vector<Complex> transmit(const std::vector<Complex>& symbols) const;

    int get_nfft()      const { return nfft_; }
    int get_cp_len()    const { return cp_len_; }
    int get_symbol_len()const { return nfft_ + cp_len_; }
    int get_n_data_sc() const { return n_data_sc_; }

private:
    int nfft_;
    int cp_len_;
    int n_data_sc_;
};
