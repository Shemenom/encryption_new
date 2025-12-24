#include <iostream>
#include <chrono>
#include <functional>
#include <fstream>
#include "algorithm.h"

using namespace std::chrono;
using namespace std;

// это не трогать !!!
template<typename Func, typename... Args>
auto Time(Func&& func, Args&&... args) {
    auto start = chrono::high_resolution_clock::now();
    
    std::invoke(forward<Func>(func), forward<Args>(args)...);
    
    auto end = high_resolution_clock::now();
    return chrono::duration_cast< microseconds>(end - start);
}

vector<string> readTestDataFromFile(const string& filename) {
    vector<string> testData;
    ifstream file(filename);
    
    string line;
    while (file >> line) {
        testData.push_back(line);
    }

    file.close();
    return testData;
}

int main() {
    setRussianLocale();

    /// Александра Долгачева - тестирование поточного шифрования ARC4
    cout << "\n=== ТЕСТИРОВАНИЕ ПОТОЧНОГО ШИФРОВАНИЯ (ARC4) ===" << endl;

    std::string message0 = "Secret Message";
    demonstrateStreamCipher(message0);

    cout << "=== ТЕСТИРОВАНИЕ ПОТОЧНОГО ШИФРОВАНИЯ ЗАВЕРШЕНО ===" << endl;

    // Шипкова Виктория (блочное шифрование RC6)
    string message1 = "Hello, world!";
    string message2 = "Hello, world! This is my block cipher implementation program! Let's check how it works, I hope it's correct!!!Hello, world! This is my block cipher implementation program! Let's check how it works, I hope it's correct!!!Hello, world! This is my block cipher implementation program! Let's check how it works, I hope it's correct!!!Hello, world! This is my block cipher implementation program! Let's check how it works, I hope it's correct!!!Hello, world! This is my block cipher implementation program! Let's check how it works, I hope it's correct!!!Hello, world! This is my block cipher implementation program! Let's check how it works, I hope it's correct!!!";

    block_cipher_RC6(message1);
    block_cipher_RC6(message2);


    return 0;
}