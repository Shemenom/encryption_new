#include "algorithm.h"
#include <cassert>
#include <stdexcept>
#include <iomanip>
#include <random>

using namespace std;

RC6::RC6(const vector<uint8_t>& K, int rounds) : r(rounds) {
    assert(K.size() == 16 || K.size() == 24 || K.size() == 32);
    KeyExpansion(K);
}

uint32_t RC6::ROTL(uint32_t x, uint32_t n) const {
    n %= w;
    return (x << n) | (x >> (w - n));
}

uint32_t RC6::ROTR(uint32_t x, uint32_t n) const {
    n %= w;
    return (x >> n) | (x << (w - n));
}

void RC6::BytesToBlock(const vector<uint8_t>& bytes, uint32_t block[4]) const {
    for (int i = 0; i < 4; ++i) {
        block[i] = bytes[u * i] |
            (bytes[u * i + 1] << 8) |
            (bytes[u * i + 2] << 16) |
            (bytes[u * i + 3] << 24);
    }
}

void RC6::BlockToBytes(const uint32_t block[4], vector<uint8_t>& bytes) const {
    bytes.resize(4 * u);
    for (int i = 0; i < 4; ++i) {
        bytes[u * i] = block[i] & 0xFF;
        bytes[u * i + 1] = (block[i] >> 8) & 0xFF;
        bytes[u * i + 2] = (block[i] >> 16) & 0xFF;
        bytes[u * i + 3] = (block[i] >> 24) & 0xFF;
    }
}

void RC6::KeyExpansion(const vector<uint8_t>& K) {

    const uint32_t Pw = 0xB7E15163U;  // P_w = Odd((e-2) * 2^w) áëèæàéøåå íå÷åòíîå öåëîå
    const uint32_t Qw = 0x9E3779B9U;  // Q_w = Odd((?-1) * 2^w)

    size_t b = K.size();
    size_t c = (b + u - 1) / u;
    if (c == 0) c = 1;

    vector<uint32_t> L(c, 0);
    for (size_t i = 0; i < b; ++i) {
        L[i / u] = (L[i / u] << 8) | K[b - 1 - i];
    }

    size_t t = 2 * r + 4;
    S.assign(t, 0);
    S[0] = Pw;
    for (size_t i = 1; i < t; ++i) {
        S[i] = S[i - 1] + Qw;
    }

    size_t v = 3 * max(c, t);
    uint32_t A = 0, B = 0;
    size_t i = 0, j = 0;

    for (size_t s = 0; s < v; ++s) {
        A = S[i] = ROTL(S[i] + A + B, 3);
        B = L[j] = ROTL(L[j] + A + B, (A + B) % w);
        i = (i + 1) % t;
        j = (j + 1) % c;
    }
}

vector<uint8_t> RC6::EncryptBlock(const vector<uint8_t>& plaintext) const {
    assert(plaintext.size() == 16);

    uint32_t A, B, C, D;
    uint32_t block[4];
    BytesToBlock(plaintext, block);

    A = block[0]; B = block[1]; C = block[2]; D = block[3];

    B += S[0];
    D += S[1];

    for (int i = 1; i <= r; ++i) {
        uint32_t t = ROTL(B * (2 * B + 1), lgw);
        uint32_t u = ROTL(D * (2 * D + 1), lgw);

        A = ROTL(A ^ t, u) + S[2 * i];
        C = ROTL(C ^ u, t) + S[2 * i + 1];

        uint32_t temp = A;
        A = B; B = C; C = D; D = temp;
    }

    A += S[2 * r + 2];
    C += S[2 * r + 3];

    block[0] = A; block[1] = B; block[2] = C; block[3] = D;

    vector<uint8_t> ciphertext;
    BlockToBytes(block, ciphertext);
    return ciphertext;
}

vector<uint8_t> RC6::DecryptBlock(const vector<uint8_t>& ciphertext) const {
    assert(ciphertext.size() == 16);

    uint32_t A, B, C, D;
    uint32_t block[4];
    BytesToBlock(ciphertext, block);

    A = block[0]; B = block[1]; C = block[2]; D = block[3];

    C -= S[2 * r + 3];
    A -= S[2 * r + 2];


    for (int i = r; i >= 1; --i) {

        uint32_t temp = D;
        D = C; C = B; B = A; A = temp;

        uint32_t u = ROTL(D * (2 * D + 1), lgw);
        uint32_t t = ROTL(B * (2 * B + 1), lgw);

        C = ROTR(C - S[2 * i + 1], t) ^ u;
        A = ROTR(A - S[2 * i], u) ^ t;
    }

    D -= S[1];
    B -= S[0];

    block[0] = A; block[1] = B; block[2] = C; block[3] = D;

    vector<uint8_t> plaintext;
    BlockToBytes(block, plaintext);
    return plaintext;
}

vector<uint8_t> RC6::Pad(const vector<uint8_t>& data) const {
    size_t padLen = 16 - (data.size() % 16);
    vector<uint8_t> padded = data;
    padded.resize(data.size() + padLen, static_cast<uint8_t>(padLen));
    return padded;
}

vector<uint8_t> RC6::Unpad(const vector<uint8_t>& data) const {
    if (data.empty()) return data;
    uint8_t padLen = data.back();
    if (padLen > 16 || padLen == 0) throw runtime_error("Invalid padding");
    for (size_t i = data.size() - padLen; i < data.size(); ++i) {
        if (data[i] != padLen) throw runtime_error("Invalid padding");
    }
    return vector<uint8_t>(data.begin(), data.end() - padLen);
}

vector<uint8_t> RC6::GenerateIV() const {
    vector<uint8_t> iv(16);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    for (auto& b : iv) b = dis(gen);
    return iv;
}

vector<uint8_t> RC6::EncryptCBC(const vector<uint8_t>& plaintext) const {
    auto padded = Pad(plaintext);
    auto iv = GenerateIV();
    vector<uint8_t> ciphertext = iv;
    vector<uint8_t> prevBlock = iv;

    for (size_t i = 0; i < padded.size(); i += 16) {
        vector<uint8_t> block(16);
        for (int j = 0; j < 16; ++j) {
            block[j] = padded[i + j] ^ prevBlock[j];
        }

        auto encryptedBlock = EncryptBlock(block);
        ciphertext.insert(ciphertext.end(), encryptedBlock.begin(), encryptedBlock.end());
        prevBlock = encryptedBlock;
    }
    return ciphertext;
}

vector<uint8_t> RC6::DecryptCBC(const vector<uint8_t>& ciphertext) const {
    if (ciphertext.size() < 16 || ciphertext.size() % 16 != 0)
        throw runtime_error("Invalid ciphertext length");

    vector<uint8_t> iv(ciphertext.begin(), ciphertext.begin() + 16);
    vector<uint8_t> encrypted(ciphertext.begin() + 16, ciphertext.end());
    vector<uint8_t> plaintext;
    vector<uint8_t> prevBlock = iv;

    for (size_t i = 0; i < encrypted.size(); i += 16) {
        auto block = vector<uint8_t>(encrypted.begin() + i, encrypted.begin() + i + 16);
        auto decryptedBlock = DecryptBlock(block);

        for (int j = 0; j < 16; ++j) {
            decryptedBlock[j] ^= prevBlock[j];
        }

        plaintext.insert(plaintext.end(), decryptedBlock.begin(), decryptedBlock.end());
        prevBlock = block;
    }

    return Unpad(plaintext);
}

void block_cipher_RC6(const std::string& message) {
    cout << "\n=== ÄÅÌÎÍÑÒÐÀÖÈß ÁËÎ×ÍÎÃÎ ØÈÔÐÎÂÀÍÈß RC6 ===" << endl;

    vector<uint8_t> K(32);
    for (int i = 0; i < 32; ++i) {
        K[i] = i + 1;
    }

    RC6 cipher(K, 20);

    if (message.size() < 40) {
        cout << "Èñõîäíîå ñîîáùåíèå: " << message << endl;
    }
    else {
        cout << "Èñõîäíîå ñîîáùåíèå: ..." << endl;
    }

    vector<uint8_t> plaintext(message.begin(), message.end());

    auto ciphertext = cipher.EncryptCBC(plaintext);
    cout << "Ñîîáùåíèå çàøèôðîâàíî â ðåæèìå CBC" << endl;

    auto decrypted = cipher.DecryptCBC(ciphertext);
    string result(decrypted.begin(), decrypted.end());

    if (result.size() <= 40) {
        cout << "Ðàñøèôðîâàííîå ñîîáùåíèå: " << result << endl;
    }
    else {
        cout << "Ðàñøèôðîâàííîå ñîîáùåíèå: ..." << endl;
    }

    if (plaintext == decrypted) {
        cout << "ØÈÔÐÎÂÀÍÈÅ RC6 ÐÀÁÎÒÀÅÒ ÊÎÐÐÅÊÒÍÎ" << endl;
    }
    else {
        cout << "ÎØÈÁÊÀ Â ØÈÔÐÎÂÀÍÈÈ RC6" << endl;
    }
}
