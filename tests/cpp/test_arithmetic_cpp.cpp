/* test_arithmetic_cpp.cpp
 *
 * c++ wrapper unit tests.
 * validates array processing and operator overloading via std::ops.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#define SIMD_F128_IMPLEMENTATION
#include <complex>
#include <iostream>
#include <vector>

#include "../include/simd_f128.hpp"
#include "../include/simd_f128_array.h"
#include "../include/simd_f128_complex.hpp"

int main() {
    int failed = 0;

    // [TEST CASE] array processing
    // verifies simd-aligned block operations on arrays.
    std::cout << "running array processing tests..." << std::endl;
    // use alignas(16) array instead of std::vector to guarantee simd alignment
    // and fix the -Wignored-attributes gcc warning.
    alignas(16) simd_f128 a[100];
    alignas(16) simd_f128 b[100];
    for (int i = 0; i < 100; i++) {
        a[i] = simd_f128_from_double(1.5);
        b[i] = simd_f128_from_double(2.5);
    }
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

    // [TEST CASE] c++ operators
    // verifies overloaded arithmetic operators and standard library integrations.
    std::cout << "running c++ operators tests..." << std::endl;
    f128::float128 x(1.5);
    f128::float128 y(2.0);
    f128::float128 z = x + y;
    if (z.to_string().find("3.5") != 0) {
        std::cerr << "Float128 C++ add failed! Got " << z.to_string() << std::endl;
        failed++;
    }

    // testing std overloads for float128
    f128::float128 rad = f128::pi * 0.25;
    f128::float128 tan_val = std::tan(rad);
    if (std::abs(tan_val - 1.0) > 1e-12) {
        std::cerr << "std::tan failed! Got " << tan_val.to_string() << std::endl;
        failed++;
    }

    f128::float128 one_f(1.0);
    f128::float128 sinh_val = std::sinh(one_f);
    f128::float128 cosh_val = std::cosh(one_f);
    f128::float128 tanh_val = std::tanh(one_f);
    if (std::abs(sinh_val - (std::exp(one_f) - std::exp(-one_f)) * 0.5) > 1e-12) {
        std::cerr << "std::sinh failed! Got " << sinh_val.to_string() << std::endl;
        failed++;
    }
    if (std::abs(cosh_val - (std::exp(one_f) + std::exp(-one_f)) * 0.5) > 1e-12) {
        std::cerr << "std::cosh failed! Got " << cosh_val.to_string() << std::endl;
        failed++;
    }
    if (std::abs(tanh_val - sinh_val / cosh_val) > 1e-12) {
        std::cerr << "std::tanh failed! Got " << tanh_val.to_string() << std::endl;
        failed++;
    }

    std::cout << "[3] Testing std::complex Integration..." << std::endl;
    std::complex<double> std_c(1.0, 2.0);
    f128::complex128 f128_c(std_c);
    f128::complex128 f128_c2(3.0, 4.0);
    f128::complex128 f128_res = f128_c * f128_c2;  // -5 + 10i

    std::complex<double> std_res = static_cast<std::complex<double>>(f128_res);
    if (std_res.real() != -5.0 || std_res.imag() != 10.0) {
        std::cerr << "std::complex conversion failed! Got " << std_res.real() << " + "
                  << std_res.imag() << "i" << std::endl;
        failed++;
    }

    // testing complex math functions
    f128::complex128 comp_z(1.0, 1.0);
    f128::complex128 comp_sinh = std::sinh(comp_z);
    std::complex<double> std_sinh_res = static_cast<std::complex<double>>(comp_sinh);
    double expected_real = std::sinh(1.0) * std::cos(1.0);
    double expected_imag = std::cosh(1.0) * std::sin(1.0);
    if (std::abs(std_sinh_res.real() - expected_real) > 1e-12 ||
        std::abs(std_sinh_res.imag() - expected_imag) > 1e-12) {
        std::cerr << "complex sinh failed! Got " << std_sinh_res << std::endl;
        failed++;
    }

    if (failed == 0) {
        std::cout << "All C++ Extension Tests Passed!" << std::endl;
        return 0;
    }
    return 1;
}
