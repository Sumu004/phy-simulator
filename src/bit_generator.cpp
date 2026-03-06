#include "bit_generator.h"
#include <random>

std::vector<int> generate_bits(int n) {
    std::vector<int> bits(n);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 1);
    for (auto& b : bits) b = dist(rng);
    return bits;
}
