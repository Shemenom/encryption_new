#include <iostream>
#include <string>
#include <vector>
#include "algorithm.h"
using namespace std;

// Инициализация S-блока на основе ключа
void SimpleStreamCipher::initialize(const vector<uint8_t>& key) {
    S.resize(256);
    for (int k = 0; k < 256; k++) {
        S[k] = k;
    }

    int m = 0;
    for (int k = 0; k < 256; k++) {
        m = (m + S[k] + key[k % key.size()]) % 256;
        swap(S[k], S[m]);
    }
}

// Генерация очередного байта ключевого потока
uint8_t SimpleStreamCipher::generateKeyByte() {
    i = (i + 1) % 256;
    j = (j + S[i]) % 256;
    swap(S[i], S[j]);
    return S[(S[i] + S[j]) % 256];
}

// Конструктор - инициализирует шифр ключом
SimpleStreamCipher::SimpleStreamCipher(const string& key) : i(0), j(0) {
    vector<uint8_t> key_data(key.begin(), key.end());
    initialize(key_data);
}

// Шифрование/дешифрование
vector<char> SimpleStreamCipher::process(const vector<char>& input) {
    // ВАЖНО: Сбрасываем состояние перед обработкой!
    i = 0;
    j = 0;

    vector<char> output;
    output.reserve(input.size());

    for (char input_byte : input) {
        uint8_t key_byte = generateKeyByte();
        output.push_back(static_cast<char>(input_byte ^ key_byte));
    }

    return output;
}

// Функция демонстрации потокового шифрования ARC4
void demonstrateStreamCipher(const std::string& message0) {
    string secret_key = "MySecretKey123";

    cout << "Исходное сообщение: " << message0 << endl;
    cout << "Секретный ключ: " << secret_key << endl << endl;

    // Шифрование
    SimpleStreamCipher encryptor(secret_key);
    vector<char> text_vector(message0.begin(), message0.end());
    vector<char> encrypted = encryptor.process(text_vector);

    cout << "Зашифрованные байты: ";
    for (char c : encrypted) {
        cout << (int)(unsigned char)c << " ";
    }
    cout << endl;

    // Дешифрование (новый объект с тем же ключом)
    SimpleStreamCipher decryptor(secret_key);
    vector<char> decrypted = decryptor.process(encrypted);
    string result(decrypted.begin(), decrypted.end());

    cout << "Расшифрованное сообщение: " << result << endl;

    // Для отладки - выведем байты расшифрованного текста
    cout << "Расшифрованные байты как числа: ";
    for (char c : decrypted) {
        cout << (int)(unsigned char)c << " ";
    }
    cout << endl;

    // Проверка корректности
    bool success = (message0 == result);
    cout << endl << "Результат: " << (success ? "Успех - сообщение восстановлено!" : "Ошибка - сообщение повреждено!") << endl;

}