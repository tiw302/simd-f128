/* cpp_operator_overload.cpp
 *
 * native c++ operator overloading for 128-bit math. */

#include <iostream>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.hpp"

using namespace f128;

int main() {
    std::cout << "--- simd-f128 c++ operator overloading demo ---\n\n";

    float128 a = 1.5;
    float128 b = 2.0;
    float128 c = "3.1415926535897932384626433832795"_f128;

    std::cout << "initial values:\n";
    std::cout << "  a = " << a << "\n";
    std::cout << "  b = " << b << "\n";
    std::cout << "  c = " << c << "\n\n";

    /* expression tree internally maps to simd intrinsics */
    float128 result = (a + b) * c - (b / a);

    std::cout << "equation: (a + b) * c - (b / a)\n";
    std::cout << "result  : " << result << "\n\n";

    return 0;
}
