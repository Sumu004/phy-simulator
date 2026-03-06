"""
5G PHY Simulator — Visualization Suite
Generates:
  1. BER vs SNR (simulated vs theoretical)
  2. Constellation diagrams (QPSK + 16-QAM clean and noisy)
  3. OFDM spectrum
"""

import subprocess
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.patches import FancyArrowPatch
import os, csv

# ─── Styling ────────────────────────────────────────────────────────────────
plt.rcParams.update({
    'figure.facecolor': '#0d1117',
    'axes.facecolor':   '#161b22',
    'axes.edgecolor':   '#30363d',
    'axes.labelcolor':  '#e6edf3',
    'xtick.color':      '#8b949e',
    'ytick.color':      '#8b949e',
    'text.color':       '#e6edf3',
    'grid.color':       '#21262d',
    'grid.linestyle':   '--',
    'grid.linewidth':   0.7,
    'legend.facecolor': '#161b22',
    'legend.edgecolor': '#30363d',
    'font.family':      'monospace',
})

ACCENT_QPSK  = '#58a6ff'
ACCENT_QAM   = '#f78166'
ACCENT_THEO  = '#3fb950'
ACCENT_NOISY = '#d2a8ff'

# ─── Run simulator to collect data ──────────────────────────────────────────
def run_sweep(mod):
    result = subprocess.run(
        ['./phy_simulator', '--mod', mod, '--sweep'],
        capture_output=True, text=True, cwd='.'
    )
    snrs, bers = [], []
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) == 4:
            try:
                snr = float(parts[0])
                ber = float(parts[3])
                snrs.append(snr)
                bers.append(max(ber, 1e-7))   # avoid log(0)
            except ValueError:
                pass
    return np.array(snrs), np.array(bers)

# ─── Theoretical BER ────────────────────────────────────────────────────────
def ber_qpsk_theoretical(snr_db):
    """BER = erfc(sqrt(SNR_linear)) / 2  for QPSK in AWGN"""
    from scipy.special import erfc
    snr = 10 ** (snr_db / 10.0)
    return 0.5 * erfc(np.sqrt(snr))

def ber_qam16_theoretical(snr_db):
    """Approximate BER for Gray-coded 16-QAM in AWGN"""
    from scipy.special import erfc
    snr = 10 ** (snr_db / 10.0)
    return (3.0/8.0) * erfc(np.sqrt(snr / 10.0))

try:
    from scipy.special import erfc
    HAS_SCIPY = True
except ImportError:
    HAS_SCIPY = False

# ─── Constellation helpers ──────────────────────────────────────────────────
INV_SQRT2 = 1.0 / np.sqrt(2.0)

def qpsk_constellation():
    return np.array([ INV_SQRT2 + 1j*INV_SQRT2,
                     -INV_SQRT2 + 1j*INV_SQRT2,
                      INV_SQRT2 - 1j*INV_SQRT2,
                     -INV_SQRT2 - 1j*INV_SQRT2])

def qam16_constellation():
    scale = 1.0 / np.sqrt(10.0)
    levels = [-3, -1, 1, 3]
    pts = []
    for i in levels:
        for q in levels:
            pts.append((i + 1j*q) * scale)
    return np.array(pts)

def add_noise(symbols, snr_db):
    snr = 10 ** (snr_db / 10.0)
    n   = len(symbols)
    noise = (np.random.randn(n) + 1j*np.random.randn(n)) / np.sqrt(2 * snr)
    return symbols + noise

def scatter_noisy(ax, mod, snr_db, n_pts, color, label):
    if mod == 'qpsk':
        pts = qpsk_constellation()
    else:
        pts = qam16_constellation()
    symbols = np.random.choice(pts, n_pts)
    noisy   = add_noise(symbols, snr_db)
    ax.scatter(noisy.real, noisy.imag, s=4, alpha=0.35, color=color, linewidths=0)
    ax.scatter(pts.real,   pts.imag,   s=80, color='white', zorder=5, marker='+', linewidths=1.5)
    ax.set_title(label, fontsize=10, pad=6)
    ax.set_xlabel('I', fontsize=9)
    ax.set_ylabel('Q', fontsize=9)
    ax.grid(True, alpha=0.4)
    ax.axhline(0, color='#30363d', lw=0.8)
    ax.axvline(0, color='#30363d', lw=0.8)
    ax.set_aspect('equal')

# ─── OFDM spectrum helper ────────────────────────────────────────────────────
def ofdm_spectrum(nfft=64, n_data_sc=48):
    """Simulate frequency-domain magnitude of one OFDM symbol"""
    freqs = np.zeros(nfft, dtype=complex)
    half  = n_data_sc // 2
    symbols = (np.random.randn(n_data_sc) + 1j*np.random.randn(n_data_sc)) / np.sqrt(2)
    freqs[1:half+1]           = symbols[:half]
    freqs[nfft-half:nfft]     = symbols[half:]
    time = np.fft.ifft(freqs)
    # Back to frequency for spectrum plot
    spectrum = np.fft.fftshift(np.abs(np.fft.fft(time))**2)
    return spectrum

# ═══════════════════════════════════════════════════════════════════════════
#  BUILD FIGURE
# ═══════════════════════════════════════════════════════════════════════════
print("Running simulations...")
snr_q, ber_q   = run_sweep('qpsk')
snr_16, ber_16 = run_sweep('qam16')
print("Simulations done.")

fig = plt.figure(figsize=(18, 11))
fig.suptitle('5G PHY Simulator — Performance Dashboard', fontsize=15,
             fontweight='bold', color='#e6edf3', y=0.97)

gs = gridspec.GridSpec(2, 3, figure=fig, hspace=0.42, wspace=0.38,
                       left=0.06, right=0.97, top=0.91, bottom=0.07)

# ── Panel 1: BER vs SNR ──────────────────────────────────────────────────────
ax_ber = fig.add_subplot(gs[0, :2])

ax_ber.semilogy(snr_q,  ber_q,  'o-', color=ACCENT_QPSK, lw=2, ms=7,
                label='QPSK (simulated)',   zorder=4)
ax_ber.semilogy(snr_16, ber_16, 's-', color=ACCENT_QAM,  lw=2, ms=7,
                label='16-QAM (simulated)', zorder=4)

snr_range = np.linspace(0, 20, 200)
if HAS_SCIPY:
    ax_ber.semilogy(snr_range, ber_qpsk_theoretical(snr_range),
                    '--', color=ACCENT_QPSK, lw=1.2, alpha=0.65,
                    label='QPSK (theoretical)')
    ax_ber.semilogy(snr_range, ber_qam16_theoretical(snr_range),
                    '--', color=ACCENT_QAM, lw=1.2, alpha=0.65,
                    label='16-QAM (theoretical)')

ax_ber.set_xlabel('SNR (dB)', fontsize=11)
ax_ber.set_ylabel('Bit Error Rate (BER)', fontsize=11)
ax_ber.set_title('BER vs SNR — AWGN Channel', fontsize=12, fontweight='bold')
ax_ber.legend(fontsize=9, loc='upper right')
ax_ber.set_xlim(-0.5, 20.5)
ax_ber.set_ylim(1e-6, 1.0)
ax_ber.grid(True, which='both', alpha=0.4)
ax_ber.tick_params(labelsize=9)

# annotation
ax_ber.annotate('QPSK needs ~6 dB less\nSNR for same BER',
                xy=(8, 2e-3), xytext=(12, 5e-2),
                arrowprops=dict(arrowstyle='->', color='#8b949e', lw=1.2),
                fontsize=8.5, color='#8b949e')

# ── Panel 2: OFDM Spectrum ────────────────────────────────────────────────────
ax_spec = fig.add_subplot(gs[0, 2])
nfft = 64
spec = ofdm_spectrum(nfft, 48)
freqs = np.arange(-nfft//2, nfft//2)
ax_spec.fill_between(freqs, 10*np.log10(spec + 1e-12),
                     alpha=0.35, color=ACCENT_QPSK)
ax_spec.plot(freqs, 10*np.log10(spec + 1e-12), color=ACCENT_QPSK, lw=1.2)
ax_spec.set_xlabel('Subcarrier Index', fontsize=10)
ax_spec.set_ylabel('Power (dBW)', fontsize=10)
ax_spec.set_title('OFDM Spectrum\n(64-FFT, 48 data subcarriers)', fontsize=10, fontweight='bold')
ax_spec.axvline(-24, color='#f78166', lw=1, ls=':', alpha=0.8, label='Data band')
ax_spec.axvline( 24, color='#f78166', lw=1, ls=':', alpha=0.8)
ax_spec.legend(fontsize=8)
ax_spec.grid(True, alpha=0.4)
ax_spec.tick_params(labelsize=8)

# ── Panel 3: QPSK Constellation (noisy) ─────────────────────────────────────
ax_q7  = fig.add_subplot(gs[1, 0])
ax_q15 = fig.add_subplot(gs[1, 1])
scatter_noisy(ax_q7,  'qpsk', 7,  2000, ACCENT_QPSK,  'QPSK @ 7 dB SNR')
scatter_noisy(ax_q15, 'qpsk', 15, 2000, ACCENT_NOISY, 'QPSK @ 15 dB SNR')

# ── Panel 4: 16-QAM Constellation ───────────────────────────────────────────
ax_qam = fig.add_subplot(gs[1, 2])
scatter_noisy(ax_qam, 'qam16', 14, 3000, ACCENT_QAM, '16-QAM @ 14 dB SNR')

# ── Watermark ────────────────────────────────────────────────────────────────
fig.text(0.5, 0.01, '5G PHY Simulator  •  OFDM + QPSK/16-QAM + AWGN  •  64-FFT  •  CP=16',
         ha='center', fontsize=8, color='#484f58')

os.makedirs('results', exist_ok=True)
out = 'results/phy_dashboard.png'
plt.savefig(out, dpi=150, bbox_inches='tight', facecolor='#0d1117')
print(f"Saved → {out}")
