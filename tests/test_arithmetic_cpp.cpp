#define SIMD_F128_IMPLEMENTATION
#include "../simd_f128.hpp"
#include "../simd_f128_complex.hpp"
#include "../simd_f128_array.h"
#include <iostream>
#include <vector>
#include <complex>

int main() {
    int failed = 0;
    
    std::cout << "[1] Testing Array Processing..." << std::endl;
    // use alignas(16) array instead of std::vector to guarantee simd alignment 
    // and fix the -Wignored-attributes gcc warning.
    alignas(16) simd_f128 a[100];
    alignas(16) simd_f128 b[100];
    for (int i=0; i<100; i++) { a[i] = simd_f128_from_double(1.5); b[i] = simd_f128_from_double(2.5); }
    alignas(16) simd_f128 out[100];
    
    simd_f128_array_add(a, b, out, 100);
    
    double hi, lo;
    simd_f128_extract(out[50], &hi, &lo);
    if (hi != 4.0) {
        std::cerr << "Array add failed! Expected 4.0, got " << hi << std::endl;
        failed++;
    }
    
    simd_f128_array_mul(a, b, out, 100);
    simd_f128_extract(out[50], &hi, &lo);
    if (hi != 3.75) {
        std::cerr << "Array mul failed! Expected 3.75, got " << hi << std::endl;
        failed++;
    }

    std::cout << "[2] Testing C++ Operators..." << std::endl;
    f128::float128 x(1.5);
    f128::float128 y(2.0);
    f128::float128 z = x + y;
    if (z.to_string().find("3.5") != 0) {
        std::cerr << "Float128 C++ add failed! Got " << z.to_string() << std::endl;
        failed++;
    }

    std::cout << "[3] Testing std::complex Integration..." << std::endl;
    std::complex<double> std_c(1.0, 2.0);
    f128::complex128 f128_c(std_c);
    f128::complex128 f128_c2(3.0, 4.0);
    f128::complex128 f128_res = f128_c * f128_c2; // -5 + 10i
    
    std::complex<double> std_res = static_cast<std::complex<double>>(f128_res);
    if (std_res.real() != -5.0 || std_res.imag() != 10.0) {
        std::cerr << "std::complex conversion failed! Got " << std_res.real() << " + " << std_res.imag() << "i" << std::endl;
        failed++;
    }

    if (failed == 0) {
        std::cout << "All C++ Extension Tests Passed!" << std::endl;
        return 0;
    }
    return 1;
}
