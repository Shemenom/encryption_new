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


