/* test_ieee754_cpp.cpp
 *
 * ieee-754 conformance tests for c++ wrappers.
 * validates c++ std::ops and object behavior with nans and infinities.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.hpp"
#include <iostream>
#include <cmath>

int test_nan_ops() {
    f128::float128 nan_val = f128::float128(NAN);
    f128::float128 norm = f128::float128(42.0);
    
    // [TEST CASE] nan propagation in operator+
    f128::float128 sum = nan_val + norm;
    if (sum.to_string().find("nan") == std::string::npos && sum.to_string().find("NaN") == std::string::npos) return 0;
    
    // [TEST CASE] nan propagation in operator*
    f128::float128 prod = nan_val * norm;
    if (prod.to_string().find("nan") == std::string::npos && prod.to_string().find("NaN") == std::string::npos) return 0;
    
    return 1;
}

int test_infinity_ops() {
    f128::float128 inf = f128::float128(INFINITY);
    f128::float128 norm = f128::float128(42.0);
    f128::float128 zero = f128::float128(0.0);
    
    // [TEST CASE] inf + norm = inf
    f128::float128 sum = inf + norm;
    if (sum.to_string().find("inf") == std::string::npos && sum.to_string().find("Inf") == std::string::npos) return 0;
    
    // [TEST CASE] inf * 0 = nan
    f128::float128 prod = inf * zero;
    if (prod.to_string().find("nan") == std::string::npos && prod.to_string().find("NaN") == std::string::npos) return 0;
    
    return 1;
}

int test_comparisons() {
    f128::float128 nan_val = f128::float128(NAN);
    f128::float128 norm = f128::float128(42.0);
    
    // [TEST CASE] nan != norm
    if (nan_val == norm) return 0;
    
    // [TEST CASE] nan != nan (IEEE 754 specific requirement)
    if (nan_val == nan_val) return 0;
    
    return 1;
}

int main() {
    int passed = 0;
    int failed = 0;
    
    std::cout << "running simd-f128 c++ ieee-754 tests..." << std::endl;

    if (test_nan_ops()) {
        std::cout << "  [PASS] test_nan_ops" << std::endl; passed++;
    } else {
        std::cout << "  [FAIL] test_nan_ops" << std::endl; failed++;
    }

    if (test_infinity_ops()) {
        std::cout << "  [PASS] test_infinity_ops" << std::endl; passed++;
    } else {
        std::cout << "  [FAIL] test_infinity_ops" << std::endl; failed++;
    }

    if (test_comparisons()) {
        std::cout << "  [PASS] test_comparisons" << std::endl; passed++;
    } else {
        std::cout << "  [FAIL] test_comparisons" << std::endl; failed++;
    }

    std::cout << "\nresults: " << passed << " passed, " << failed << " failed" << std::endl;
    return (failed > 0) ? 1 : 0;
}
