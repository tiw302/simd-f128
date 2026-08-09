/* test_ieee754.c
 *
 * ieee-754 conformance and edge-case unit tests.
 * validates signed zero, nan payloads, infinity arithmetic, and catastrophic cancellation.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMD_F128_IMPLEMENTATION
#include "../../include/simd_f128.h"
#include "../../include/simd_f128_math.h"
#include "../../include/simd_f128_utils.h"

// [TEST CASE] signed zero
// verifies +0.0 and -0.0 distinguishability and interactions.
int test_signed_zero() {
    simd_f128 pos_zero = simd_f128_from_double(0.0);
    simd_f128 neg_zero = simd_f128_from_double(-0.0);
    simd_f128 one = simd_f128_from_double(1.0);

    // 1.0 / -0.0 should be -inf
    simd_f128 neg_inf = simd_f128_div(one, neg_zero);
    double hi, lo;
    simd_f128_extract(neg_inf, &hi, &lo);
    if (!isinf(hi) || hi > 0.0) return 0;

    // 1.0 / +0.0 should be +inf
    simd_f128 pos_inf = simd_f128_div(one, pos_zero);
    simd_f128_extract(pos_inf, &hi, &lo);
    if (!isinf(hi) || hi < 0.0) return 0;

    // -0.0 + 0.0 = +0.0
    simd_f128 sum_zero = simd_f128_add(neg_zero, pos_zero);
    simd_f128_extract(sum_zero, &hi, &lo);
    if (signbit(hi)) return 0;

    return 1;
}

// [TEST CASE] nan propagation
// verifies that nan taints subsequent operations.
int test_nan_propagation() {
    simd_f128 nan_val = simd_f128_from_double(NAN);
    simd_f128 norm = simd_f128_from_double(42.0);

    simd_f128 res1 = simd_f128_add(nan_val, norm);
    simd_f128 res2 = simd_f128_mul(nan_val, norm);
    simd_f128 res3 = simd_f128_sub(norm, nan_val);

    double h1, l1, h2, l2, h3, l3;
    simd_f128_extract(res1, &h1, &l1);
    simd_f128_extract(res2, &h2, &l2);
    simd_f128_extract(res3, &h3, &l3);

    if (!isnan(h1)) return 0;
    if (!isnan(h2)) return 0;
    if (!isnan(h3)) return 0;

    return 1;
}

// [TEST CASE] infinity arithmetic
// verifies behavior of infinities under various operations.
int test_infinity_arithmetic() {
    simd_f128 inf = simd_f128_from_double(INFINITY);
    simd_f128 norm = simd_f128_from_double(42.0);
    simd_f128 zero = simd_f128_from_double(0.0);

    // inf + norm = inf
    simd_f128 r1 = simd_f128_add(inf, norm);
    double h1, l1;
    simd_f128_extract(r1, &h1, &l1);
    if (!isinf(h1)) return 0;

    // inf - inf = nan
    simd_f128 r2 = simd_f128_sub(inf, inf);
    double h2, l2;
    simd_f128_extract(r2, &h2, &l2);
    if (!isnan(h2)) return 0;

    // inf * 0 = nan
    simd_f128 r3 = simd_f128_mul(inf, zero);
    double h3, l3;
    simd_f128_extract(r3, &h3, &l3);
    if (!isnan(h3)) return 0;

    return 1;
}

// [TEST CASE] catastrophic cancellation
// verifies precision retention near the limits of double-double mantissa (106 bits).
int test_catastrophic_cancellation() {
    // 2^106 = 8.112963841460668e31
    // (A + B) - A
    simd_f128 a = simd_f128_from_double(1e30);
    simd_f128 b =
        simd_f128_from_double(1.0);  // B is much smaller than A, but still within 106 bits

    simd_f128 sum = simd_f128_add(a, b);
    simd_f128 diff = simd_f128_sub(sum, a);  // should recover exactly 1.0

    double hi, lo;
    simd_f128_extract(diff, &hi, &lo);
    if (fabs(hi - 1.0) > 1e-15) return 0;

    return 1;
}

int main() {
    int passed = 0;
    int failed = 0;

    printf("running simd-f128 ieee-754 conformance tests...\n");

    if (test_signed_zero()) {
        printf("  [PASS] test_signed_zero\n");
        passed++;
    } else {
        printf("  [FAIL] test_signed_zero\n");
        failed++;
    }

    if (test_nan_propagation()) {
        printf("  [PASS] test_nan_propagation\n");
        passed++;
    } else {
        printf("  [FAIL] test_nan_propagation\n");
        failed++;
    }

    if (test_infinity_arithmetic()) {
        printf("  [PASS] test_infinity_arithmetic\n");
        passed++;
    } else {
        printf("  [FAIL] test_infinity_arithmetic\n");
        failed++;
    }

    if (test_catastrophic_cancellation()) {
        printf("  [PASS] test_catastrophic_cancellation\n");
        passed++;
    } else {
        printf("  [FAIL] test_catastrophic_cancellation\n");
        failed++;
    }

    printf("\nresults: %d passed, %d failed\n", passed, failed);
    return (failed > 0) ? 1 : 0;
}
