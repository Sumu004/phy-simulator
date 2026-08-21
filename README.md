# 5G PHY Simulator

A 4G/5G physical layer DSP simulator in C++17. Bits go through QPSK or 16-QAM modulation, OFDM (FFT + cyclic prefix), an AWGN channel, then back through demodulation to measure bit error rate. No external DSP libraries, the FFT is built from scratch.

```
Bits → Modulation (QPSK/16-QAM) → OFDM Tx → AWGN Channel → OFDM Rx → Demodulation → BER
```

## Build

```bash
make
```

Needs `g++` with C++17. Nothing else to install.

## Usage

```bash
./phy_simulator --mod qpsk  --snr 10
./phy_simulator --mod qam16 --snr 15
./phy_simulator --mod qpsk  --sweep
python3 visualize.py
```

## OFDM parameters

64-point FFT, 16-sample cyclic prefix, 48 data subcarriers, 16 guard subcarriers.

## Results

BER for QPSK over AWGN:

| SNR (dB) | BER |
|---|---|
| 0 | ~0.12 |
| 4 | ~0.03 |
| 8 | ~0.002 |
| 10 | ~0.0002 |
| 12+ | ~0 |

Matches the theoretical curve: BER = 0.5 * erfc(sqrt(SNR)) for QPSK, (3/8) * erfc(sqrt(SNR/10)) for 16-QAM.
