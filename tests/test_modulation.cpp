/**
 * tests/test_modulation.cpp
 *
 * Lightweight unit tests — no external test framework needed.
 * Build: g++ -std=c++17 -O2 -I../src tests/test_modulation.cpp
 *        ../src/fft.cpp ../src/bit_generator.cpp
 *        ../src/modulation/qpsk.cpp ../src/modulation/qam16.cpp
 *        ../src/ber.cpp -o run_tests -lm
 * Run:   ./run_tests
 */

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cassert>
#include <string>

#include "fft.h"
#include "bit_generator.h"
#include "modulation/qpsk.h"
#include "modulation/qam16.h"
#include "ber.h"

// ─── Test helpers ────────────────────────────────────────────────────────────
static int tests_run = 0, tests_passed = 0;

#define TEST(name) \
    do { std::cout << "  " << (name) << " ... "; tests_run++; } while(0)

#define PASS() \
    do { std::cout << "PASS\n"; tests_passed++; } while(0)

#define FAIL(msg) \
    do { std::cout << "FAIL — " << msg << "\n"; } while(0)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { FAIL(#a " != " #b); return; }

#define ASSERT_NEAR(a, b, tol) \
    if (std::abs((a) - (b)) > (tol)) { \
        FAIL(#a " = " << (a) << ", expected " << (b)); return; \
    }

// ─── FFT tests ───────────────────────────────────────────────────────────────
void test_fft_roundtrip() {
    TEST("FFT → IFFT roundtrip");
    CVector original = {{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0}};
    CVector x = original;
    fft(x);
    ifft(x);
    for (size_t i = 0; i < x.size(); i++) {
        if (std::abs(x[i].real() - original[i].real()) > 1e-9 ||
            std::abs(x[i].imag() - original[i].imag()) > 1e-9) {
            FAIL("roundtrip mismatch at index " << i); return;
        }
    }
    PASS();
}

void test_fft_known_output() {
    TEST("FFT of [1,0,0,0] = [1,1,1,1]");
    CVector x = {{1,0},{0,0},{0,0},{0,0}};
    fft(x);
    for (int i = 0; i < 4; i++) {
        ASSERT_NEAR(x[i].real(), 1.0, 1e-9);
        ASSERT_NEAR(x[i].imag(), 0.0, 1e-9);
    }
    PASS();
}

// ─── QPSK tests ──────────────────────────────────────────────────────────────
void test_qpsk_symbol_count() {
    TEST("QPSK: 8 bits → 4 symbols");
    auto bits = std::vector<int>{0,0, 0,1, 1,0, 1,1};
    auto syms = QPSK::modulate(bits);
    ASSERT_EQ((int)syms.size(), 4);
    PASS();
}

void test_qpsk_known_mapping() {
    TEST("QPSK: 00→(+,+)  01→(-,+)  10→(+,-)  11→(-,-)");
    double s = 1.0 / std::sqrt(2.0);
    auto bits = std::vector<int>{0,0, 0,1, 1,0, 1,1};
    auto syms = QPSK::modulate(bits);
    auto check = [&](int idx, double re, double im) {
        return std::abs(syms[idx].real() - re) < 1e-9 &&
               std::abs(syms[idx].imag() - im) < 1e-9;
    };
    if (!check(0, s, s) || !check(1,-s, s) || !check(2, s,-s) || !check(3,-s,-s)) {
        FAIL("constellation mapping wrong"); return;
    }
    PASS();
}

void test_qpsk_roundtrip() {
    TEST("QPSK: modulate → demodulate = identity (no noise)");
    auto tx_bits = generate_bits(200);
    // pad to even
    if (tx_bits.size() % 2) tx_bits.push_back(0);
    auto syms    = QPSK::modulate(tx_bits);
    auto rx_bits = QPSK::demodulate(syms);
    ASSERT_EQ(tx_bits.size(), rx_bits.size());
    int errs = count_errors(tx_bits, rx_bits);
    ASSERT_EQ(errs, 0);
    PASS();
}

// ─── 16-QAM tests ────────────────────────────────────────────────────────────
void test_qam16_symbol_count() {
    TEST("16-QAM: 16 bits → 4 symbols");
    auto bits = generate_bits(16);
    auto syms = QAM16::modulate(bits);
    ASSERT_EQ((int)syms.size(), 4);
    PASS();
}

void test_qam16_roundtrip() {
    TEST("16-QAM: modulate → demodulate = identity (no noise)");
    auto tx_bits = generate_bits(200);
    // pad to multiple of 4
    while (tx_bits.size() % 4) tx_bits.push_back(0);
    auto syms    = QAM16::modulate(tx_bits);
    auto rx_bits = QAM16::demodulate(syms);
    ASSERT_EQ(tx_bits.size(), rx_bits.size());
    int errs = count_errors(tx_bits, rx_bits);
    ASSERT_EQ(errs, 0);
    PASS();
}

void test_qam16_unit_power() {
    TEST("16-QAM: average constellation power ≈ 1.0");
    double total = 0;
    for (int i = 0; i < 16; i++) total += std::norm(QAM16::constellation(i));
    double avg = total / 16.0;
    ASSERT_NEAR(avg, 1.0, 1e-9);
    PASS();
}

// ─── BER tests ───────────────────────────────────────────────────────────────
void test_ber_zero() {
    TEST("BER: identical vectors → 0.0");
    auto bits = std::vector<int>{1,0,1,1,0,0,1,0};
    auto r = compute_ber(10.0, bits, bits);
    ASSERT_EQ(r.error_bits, 0);
    ASSERT_NEAR(r.ber, 0.0, 1e-12);
    PASS();
}

void test_ber_all_wrong() {
    TEST("BER: all bits flipped → 1.0");
    auto tx = std::vector<int>{1,1,1,1};
    auto rx = std::vector<int>{0,0,0,0};
    auto r = compute_ber(0.0, tx, rx);
    ASSERT_EQ(r.error_bits, 4);
    ASSERT_NEAR(r.ber, 1.0, 1e-12);
    PASS();
}

// ─── main ────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "\n=== PHY Simulator Unit Tests ===\n\n";

    std::cout << "FFT:\n";
    test_fft_roundtrip();
    test_fft_known_output();

    std::cout << "\nQPSK:\n";
    test_qpsk_symbol_count();
    test_qpsk_known_mapping();
    test_qpsk_roundtrip();

    std::cout << "\n16-QAM:\n";
    test_qam16_symbol_count();
    test_qam16_roundtrip();
    test_qam16_unit_power();

    std::cout << "\nBER:\n";
    test_ber_zero();
    test_ber_all_wrong();

    std::cout << "\n────────────────────────────────\n";
    std::cout << "Results: " << tests_passed << "/" << tests_run << " passed\n";

    if (tests_passed == tests_run) {
        std::cout << "ALL TESTS PASSED ✓\n\n";
        return 0;
    } else {
        std::cout << (tests_run - tests_passed) << " TESTS FAILED ✗\n\n";
        return 1;
    }
}
