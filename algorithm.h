#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <tuple>
using namespace std;

inline void setRussianLocale() {
    setlocale(LC_ALL, "Russian");
}

// Александра Долгачева (поточное шифрование ARC4)
class SimpleStreamCipher {
private:
    vector<uint8_t> S; // S-блок 256 байт
    int i, j;

    void initialize(const vector<uint8_t>& key);
    uint8_t generateKeyByte();

public:
    SimpleStreamCipher(const std::string& key);
    vector<char> process(const vector<char>& input);
};

void demonstrateStreamCipher(const std::string& message0);

// Шипкова Виктория (блочное шифрование RC6)
class RC6 {
private:

    uint32_t ROTL(uint32_t x, uint32_t n) const;
    uint32_t ROTR(uint32_t x, uint32_t n) const;

    void KeyExpansion(const vector<uint8_t>& K);

    void BytesToBlock(const vector<uint8_t>& bytes, uint32_t block[4]) const;
    void BlockToBytes(const uint32_t block[4], vector<uint8_t>& bytes) const;

    vector<uint8_t> Pad(const vector<uint8_t>& data) const;
    vector<uint8_t> Unpad(const vector<uint8_t>& data) const;
    vector<uint8_t> GenerateIV() const;

    static constexpr int w = 32;
    static constexpr int u = w / 8;
    static constexpr uint32_t lgw = 5;

    int r;
    vector<uint32_t> S;

public:
    RC6(const vector<uint8_t>& K, int rounds = 20);

    vector<uint8_t> EncryptBlock(const vector<uint8_t>& plaintext) const;
    vector<uint8_t> DecryptBlock(const vector<uint8_t>& ciphertext) const;

    vector<uint8_t> EncryptCBC(const vector<uint8_t>& plaintext) const;
    vector<uint8_t> DecryptCBC(const vector<uint8_t>& ciphertext) const;
};

void block_cipher_RC6(const string& message);

