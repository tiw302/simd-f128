#include <iostream>

struct MyFloat { double v; };

MyFloat operator""_f128(const char* str) {
    std::cout << "raw: " << str << "\n";
    return {1.0};
}

MyFloat operator""_f128(const char* str, std::size_t len) {
    std::cout << "str: " << str << " len: " << len << "\n";
    return {2.0};
}

int main() {
    auto a = 1.234_f128;
    auto b = "1.234"_f128;
    return 0;
}
