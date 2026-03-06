# 5G PHY Simulator

A real-time Physical Layer (PHY) simulator implementing core 5G/LTE air interface DSP.

## What It Simulates

```
Input Bits → Channel Coding → Modulation (QPSK/16-QAM)
          → OFDM Modulator → AWGN Channel
          → OFDM Demodulator → Demodulation → Bit Recovery → BER
```

## Components

| File | What it does |
|---|---|
| `src/bit_generator.cpp` | Random bit generation |
| `src/fft.cpp` | Cooley-Tukey radix-2 DIT FFT/IFFT |
| `src/modulation/qpsk.cpp` | QPSK modulation/demodulation |
| `src/modulation/qam16.cpp` | 16-QAM (Gray-coded) mod/demod |
| `src/ofdm_tx.cpp` | OFDM transmitter (IFFT + cyclic prefix) |
| `src/ofdm_rx.cpp` | OFDM receiver (remove CP + FFT) |
| `src/channel.cpp` | AWGN channel model |
| `src/ber.cpp` | Bit Error Rate measurement |
| `src/main.cpp` | CLI entry point |
| `visualize.py` | BER curve + constellation + spectrum plots |

## OFDM Parameters

| Parameter | Value |
|---|---|
| FFT Size | 64 |
| Cyclic Prefix | 16 samples |
| Data Subcarriers | 48 |
| Guard Subcarriers | 16 |

## Build

```bash
make
```

Requirements: `g++` with C++17 support. No external libraries needed (FFT is built-in).

## Usage

```bash
# Single SNR point
./phy_simulator --mod qpsk  --snr 10
./phy_simulator --mod qam16 --snr 15

# Sweep SNR 0–20 dB
./phy_simulator --mod qpsk  --sweep
./phy_simulator --mod qam16 --sweep

# Generate plots
python3 visualize.py
```

## Expected Output

```
=== 5G PHY Simulator ===
Modulation : QPSK
OFDM       : 64-FFT, 48 data subcarriers, CP=16
OFDM Syms  : 100

SNR        : 10.000000 dB

Bits transmitted : 9600
Errors           : 2
BER              : 0.000208
```

## BER Reference Values (QPSK, AWGN)

| SNR (dB) | Typical BER |
|---|---|
| 0 | ~0.12 |
| 4 | ~0.03 |
| 8 | ~0.002 |
| 10 | ~0.0002 |
| 12+ | ~0 |

## Theory

**QPSK** maps 2 bits per symbol onto 4 points on the unit circle.
BER (theoretical) = ½ · erfc(√SNR)

**16-QAM** maps 4 bits per symbol onto a 4×4 grid.
BER (approx) = (3/8) · erfc(√(SNR/10))

**OFDM** uses IFFT to transmit N symbols simultaneously on orthogonal subcarriers.
The cyclic prefix (CP) prevents inter-symbol interference from multipath.

**AWGN** channel adds complex Gaussian noise calibrated to achieve the specified SNR
relative to the actual transmitted signal power.
