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
    
    return 0;
}