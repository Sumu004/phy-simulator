CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Isrc
SRCS     = src/main.cpp src/fft.cpp src/bit_generator.cpp \
           src/modulation/qpsk.cpp src/modulation/qam16.cpp \
           src/ofdm_tx.cpp src/ofdm_rx.cpp src/channel.cpp src/ber.cpp
TARGET   = phy_simulator

TEST_SRCS = tests/test_modulation.cpp src/fft.cpp src/bit_generator.cpp \
            src/modulation/qpsk.cpp src/modulation/qam16.cpp src/ber.cpp
TEST_BIN  = run_tests

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm
	@echo "Build successful → ./$(TARGET)"

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm

run-qpsk:
	./$(TARGET) --mod qpsk --snr 10

run-qam16:
	./$(TARGET) --mod qam16 --snr 15

sweep-qpsk:
	./$(TARGET) --mod qpsk --sweep

sweep-qam16:
	./$(TARGET) --mod qam16 --sweep

sweep-both:
	./$(TARGET) --mod qpsk  --sweep
	./$(TARGET) --mod qam16 --sweep

plot:
	python3 python/visualize.py

clean:
	rm -f $(TARGET) $(TEST_BIN) results/*.csv results/*.png

.PHONY: all test run-qpsk run-qam16 sweep-qpsk sweep-qam16 sweep-both plot clean
