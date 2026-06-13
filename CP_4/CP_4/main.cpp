#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

inline int popcount32(uint32_t x) {
    x -= ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void L1(uint32_t& reg) {
    uint32_t t = ((reg >> 0) ^ (reg >> 1) ^ (reg >> 4) ^ (reg >> 6)) & 1;
    reg = reg >> 1;
    reg |= (t << 29);
}

void L2(uint32_t& reg) {
    uint32_t t = (reg ^ (reg >> 3)) & 1;
    reg = reg >> 1;
    reg |= (t << 30);
}

void L3(uint32_t& reg) {
    uint32_t t = (reg ^ (reg >> 1) ^ (reg >> 2) ^ (reg >> 3) ^ (reg >> 5) ^ (reg >> 7)) & 1;
    reg = reg >> 1;
    reg |= (t << 31);
}

bool F(uint32_t x, uint32_t y, uint32_t s) {
    return (s & 1) ? (x & 1) : (y & 1);
}

bool correct_key(uint32_t L1_val, uint32_t L2_val, uint32_t L3_val, const bool z[2048]) {
    uint32_t reg1 = L1_val;
    uint32_t reg2 = L2_val;
    uint32_t reg3 = L3_val;
    for (int i = 0; i < 2048; i++) {
        if (F(reg1, reg2, reg3) != z[i]) return false;
        L1(reg1);
        L2(reg2);
        L3(reg3);
    }
    return true;
}

bool find_L3(uint32_t L1_val, uint32_t L2_val, const bool z[2048], uint32_t& found_L3) {
    uint32_t base_L3 = 0;
    vector<int> unknown_bits;
    uint32_t reg1 = L1_val;
    uint32_t reg2 = L2_val;

    for (int i = 0; i < 32; ++i) {
        uint32_t x = reg1 & 1;
        uint32_t y = reg2 & 1;
        if (x != y) {
            uint32_t s = (z[i] == x) ? 1 : 0;
            base_L3 |= (s << i);
        }
        else {
            unknown_bits.push_back(i);
        }
        L1(reg1);
        L2(reg2);
    }

    size_t num_combinations = 1ULL << unknown_bits.size();
    for (size_t k = 0; k < num_combinations; ++k) {
        uint32_t candidate_L3 = base_L3;
        for (size_t i = 0; i < unknown_bits.size(); i++) {
            if ((k >> i) & 1) {
                candidate_L3 |= (1U << unknown_bits[i]);
            }
        }
        if (correct_key(L1_val, L2_val, candidate_L3, z)) {
            found_L3 = candidate_L3;
            return true;
        }
    }
    return false;
}

uint32_t gen32_L1(uint32_t& reg) {
    uint32_t out = 0;
    for (int i = 0; i < 32; i++) {
        out |= (reg & 1) << i;
        uint32_t t = ((reg) ^ (reg >> 1) ^ (reg >> 4) ^ (reg >> 6)) & 1;
        reg = (reg >> 1) | (t << 29);
    }
    return out;
}

uint32_t gen32_L2(uint32_t& reg) {
    uint32_t out = 0;
    for (int i = 0; i < 32; i++) {
        out |= (reg & 1) << i;
        uint32_t t = ((reg) ^ (reg >> 3)) & 1;
        reg = (reg >> 1) | (t << 30);
    }
    return out;
}

int main() {
    string z_str = "00000101100001010101000110010100001101110001101000001011110011010001100100000011100101011111111110101101001000110100000010000110001010101001011101011001100100101111110100010101101011011111011101000101011001110111011011001101011001011110101101110000111000000011111001000000100011110101100001100101000101010000001010010000100100011100001001101110000001110000100111000101100110110101000010010001100001100001000111011001111001111100010011101011100000000010011010100110111010101100011101101101110110011010011000100010000010111010000110111010100101011000111100101110100110101110000110011011000110001101111101111101101001111100110001011011111000101000000111101011101000100010000011001110001000100010101100010110011111111100001000101000010001110111000111001110100100000111110110111101010001101110111100111100000111110000011000011110100100001101101100110111100110001011011111110000100111110001001100010101101001101101111001101101010011111110110010111111101011110000100101100101110111011101111101010011100100011011001010010011010001010111100101101100001100101001011101100101000010000000001010110110100110011010101011011000001111001000011100101011011010010111010111001100000111001100110011101110010101010100001010111110001000100010010111001001110110110100000110000111011100100111001101011010001110011011110110011111001111011000001010100100000101011010001111101001101111000100110111000100010001111101111110110011001000100011100110000100011100010111001010100111010010110110110010000111101000110011010111110010111110011110110001111110010101011011011100010101001101001101001010000000100110010111010111111100010110110001001101101110010001010001010001110100011000111011000011110101011100011110001001000001011010011110101101011101101101001100111100010010110010100101010100010110010100010111011000001000111111001010010011011010011011100000100101100010101111000100101001011111010110101110011000000101011100010001101010100001001111100000000011111011011011110000110010011110000010111111110111110010100101010011111011000000011011101011010111110000100011100111101111101111";

    bool z_bits[2048];
    uint32_t z_words[64] = { 0 };

    for (int i = 0; i < 2048; i++) {
        z_bits[i] = z_str[i] - '0';
        if (z_bits[i]) {
            z_words[i / 32] |= (1U << (i % 32));
        }
    }

    int max_err[64];
    for (int i = 0; i < 64; ++i) {
        int n = (i + 1) * 32;
        max_err[i] = (int)(0.25 * n + 6.5 * sqrt(0.1875 * n));
    }

    uint32_t best_L1 = 0;
    int min_err_L1 = 2048;
    cout << "Searching L1..." << endl;
    for (uint32_t i = 1; i < (1UL << 30); i++) {
        uint32_t reg = i;
        int errs = 0;
        bool ok = true;
        for (int w = 0; w < 64; w++) {
            uint32_t out = gen32_L1(reg);
            errs += popcount32(out ^ z_words[w]);
            if (errs > max_err[w]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            if (errs < min_err_L1) {
                min_err_L1 = errs;
                best_L1 = i;
            }
        }
    }
    cout << "L1 = " << best_L1 << " (Errors: " << min_err_L1 << ")" << endl;

    uint32_t best_L2 = 0;
    int min_err_L2 = 2048;
    cout << "Searching L2..." << endl;
    for (uint32_t i = 1; i < (1UL << 31); i++) {
        uint32_t reg = i;
        int errs = 0;
        bool ok = true;
        for (int w = 0; w < 64; w++) {
            uint32_t out = gen32_L2(reg);
            errs += popcount32(out ^ z_words[w]);
            if (errs > max_err[w]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            if (errs < min_err_L2) {
                min_err_L2 = errs;
                best_L2 = i;
            }
        }
    }
    cout << "L2 = " << best_L2 << " (Errors: " << min_err_L2 << ")" << endl;

    uint32_t best_L3 = 0;
    cout << "Searching L3..." << endl;
    if (find_L3(best_L1, best_L2, z_bits, best_L3)) {
        cout << "L3 = " << best_L3 << endl;
        cout << ">>> GG WP! ALL KEYS FOUND <<<" << endl;
    }
    else {
        cout << "Ruin. L3 not found." << endl;
    }

    return 0;
}