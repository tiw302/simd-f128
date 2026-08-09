/* test_vector.c
 *
 * unit tests for simd_f128_vector module.
 * validates 4-lane simd_f128x4 arithmetic operations.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_math.h"
#include "../include/simd_f128_utils.h"
#include "../include/simd_f128_vector.h"

static int within_ulp(simd_f128 a, simd_f128 b, int ulps) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);
    if (ahi == bhi && alo == blo) return 1;
    double eps = 0x1.0p-52;
    double tol_hi = ulps * eps * (fabs(ahi) > 0.0 ? fabs(ahi) : 1.0);
    if (fabs(ahi - bhi) > tol_hi) return 0;
    double scale_lo = fabs(alo) > 0.0 ? fabs(alo) : (fabs(ahi) * eps);
    double tol_lo = ulps * eps * (scale_lo > 0.0 ? scale_lo : 1.0);
    if (fabs(alo - blo) > tol_lo) return 0;
    return 1;
}

static int almost_equal(simd_f128 a, simd_f128 b, int max_ulp) {
    return within_ulp(a, b, max_ulp);
}

// extract lane from simd_f128x4
static simd_f128 extract_lane(simd_f128x4 v, int lane) {
    double ahi[4], alo[4];
#if defined(SIMD_F128_USE_AVX2)
    _mm256_storeu_pd(ahi, v.hi);
    _mm256_storeu_pd(alo, v.lo);
    return simd_f128_from_hi_lo(ahi[lane], alo[lane]);
#else
    return v.val[lane];
#endif
}

// [TEST CASE] simd_f128x4 initialization
// verifies lane population.
int test_vector_init() {
    simd_f128x4 v = simd_f128x4_from_doubles(1.0, 2.0, 3.0, 4.0);

    if (!almost_equal(extract_lane(v, 0), simd_f128_from_double(1.0), 1)) return 0;
    if (!almost_equal(extract_lane(v, 1), simd_f128_from_double(2.0), 1)) return 0;
    if (!almost_equal(extract_lane(v, 2), simd_f128_from_double(3.0), 1)) return 0;
    if (!almost_equal(extract_lane(v, 3), simd_f128_from_double(4.0), 1)) return 0;
    return 1;
}

// [TEST CASE] simd_f128x4 addition
// verifies parallel addition.
int test_vector_add() {
    simd_f128x4 a = simd_f128x4_from_doubles(1.0, 2.0, 3.0, 4.0);
    simd_f128x4 b = simd_f128x4_from_doubles(5.0, 6.0, 7.0, 8.0);

    simd_f128x4 out = simd_f128x4_add(a, b);

    if (!almost_equal(extract_lane(out, 0), simd_f128_from_double(6.0), 1)) return 0;
    if (!almost_equal(extract_lane(out, 1), simd_f128_from_double(8.0), 1)) return 0;
    if (!almost_equal(extract_lane(out, 2), simd_f128_from_double(10.0), 1)) return 0;
    if (!almost_equal(extract_lane(out, 3), simd_f128_from_double(12.0), 1)) return 0;
    return 1;
}

// [TEST CASE] simd_f128x4 multiplication
// verifies parallel multiplication.
int test_vector_mul() {
    simd_f128x4 a = simd_f128x4_from_doubles(1.0, 2.0, 3.0, 4.0);
    simd_f128x4 b = simd_f128x4_from_doubles(5.0, 6.0, 7.0, 8.0);

    simd_f128x4 out = simd_f128x4_mul(a, b);

    if (!almost_equal(extract_lane(out, 0), simd_f128_from_double(5.0), 1)) return 0;
    if (!almost_equal(extract_lane(out, 1), simd_f128_from_double(12.0), 1)) return 0;
    if (!almost_equal(extract_lane(out, 2), simd_f128_from_double(21.0), 1)) return 0;
    if (!almost_equal(extract_lane(out, 3), simd_f128_from_double(32.0), 1)) return 0;
    return 1;
}

int main() {
    int passed = 0;
    int failed = 0;

    printf("running simd-f128 vector tests...\n");

    if (test_vector_init()) {
        printf("  [PASS] test_vector_init\n");
        passed++;
    } else {
        printf("  [FAIL] test_vector_init\n");
        failed++;
    }

    if (test_vector_add()) {
        printf("  [PASS] test_vector_add\n");
        passed++;
    } else {
        printf("  [FAIL] test_vector_add\n");
        failed++;
    }

    if (test_vector_mul()) {
        printf("  [PASS] test_vector_mul\n");
        passed++;
    } else {
        printf("  [FAIL] test_vector_mul\n");
        failed++;
    }

    printf("\nresults: %d passed, %d failed\n", passed, failed);
    return (failed > 0) ? 1 : 0;
}
