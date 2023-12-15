#include <iostream>
#include <bitset>

class StreamCipher {
    unsigned long long state;

public:
    StreamCipher(unsigned long long seed) : state(seed) {}

    // LCG parameters
    static constexpr unsigned long long a = 1664525;
    static constexpr unsigned long long c = 1013904223;
    static constexpr unsigned long long m = 4294967296; // 2^32

    // Generate a pseudo-random byte
    unsigned char nextByte() {
        state = (a * state + c) % m;
        return static_cast<unsigned char>(state >> 24); // Return the top 8 bits
    }
};

int main() {
    StreamCipher cipher(12345); // Seed for the generator

    // Example: Generate 10 bytes of keystream
    for(int i = 0; i < 10; i++) {
        std::bitset<8> byte(cipher.nextByte());
        std::cout << byte << ' ';
    }

    return 0;
}
