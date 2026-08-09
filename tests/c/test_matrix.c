/* test_matrix.c
 *
 * unit tests for simd_f128_matrix module.
 * validates 4x4 matrix arithmetic, identity generation, and transformations.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_matrix.h"
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

// [TEST CASE] matrix identity
// verifies that identity matrix is correctly generated.
int test_matrix_identity() {
    simd_f128_mat4 ident = simd_f128_mat4_identity();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            simd_f128 val = ident.m[i][j];
            double expected = (i == j) ? 1.0 : 0.0;
            if (!almost_equal(val, simd_f128_from_double(expected), 1)) {
                return 0;
            }
        }
    }
    return 1;
}

// [TEST CASE] matrix vector multiply
// verifies multiplication of a 4x4 matrix with a 4D vector.
int test_matrix_vector_mul() {
    simd_f128_mat4 ident = simd_f128_mat4_identity();

    // transform vector by identity
    simd_f128_vec4 vec = {simd_f128_from_double(1.0), simd_f128_from_double(2.0),
                          simd_f128_from_double(3.0), simd_f128_from_double(4.0)};
    simd_f128_vec4 out = simd_f128_mat4_mul_vec4(ident, vec);

    if (!almost_equal(out.x, vec.x, 1)) return 0;
    if (!almost_equal(out.y, vec.y, 1)) return 0;
    if (!almost_equal(out.z, vec.z, 1)) return 0;
    if (!almost_equal(out.w, vec.w, 1)) return 0;

    // transform vector by scaled matrix
    simd_f128_mat4 scaled;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            scaled.m[i][j] = (i == j) ? simd_f128_from_double(2.0) : simd_f128_from_double(0.0);
        }
    }
    out = simd_f128_mat4_mul_vec4(scaled, vec);

    if (!almost_equal(out.x, simd_f128_from_double(2.0), 1)) return 0;
    if (!almost_equal(out.y, simd_f128_from_double(4.0), 1)) return 0;
    if (!almost_equal(out.z, simd_f128_from_double(6.0), 1)) return 0;
    if (!almost_equal(out.w, simd_f128_from_double(8.0), 1)) return 0;
    return 1;
}

// [TEST CASE] matrix matrix multiply
// verifies multiplication of two 4x4 matrices.
int test_matrix_matrix_mul() {
    simd_f128_mat4 ident = simd_f128_mat4_identity();

    simd_f128_mat4 a;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            a.m[i][j] = simd_f128_from_double((double)(i * 4 + j + 1));
        }
    }

    simd_f128_mat4 out = simd_f128_mat4_mul(a, ident);

    // A * I should equal A
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!almost_equal(out.m[i][j], a.m[i][j], 1)) {
                return 0;
            }
        }
    }
    return 1;
}

int main() {
    int passed = 0;
    int failed = 0;

    printf("running simd-f128 matrix tests...\n");

    if (test_matrix_identity()) {
        printf("  [PASS] test_matrix_identity\n");
        passed++;
    } else {
        printf("  [FAIL] test_matrix_identity\n");
        failed++;
    }

    if (test_matrix_vector_mul()) {
        printf("  [PASS] test_matrix_vector_mul\n");
        passed++;
    } else {
        printf("  [FAIL] test_matrix_vector_mul\n");
        failed++;
    }

    if (test_matrix_matrix_mul()) {
        printf("  [PASS] test_matrix_matrix_mul\n");
        passed++;
    } else {
        printf("  [FAIL] test_matrix_matrix_mul\n");
        failed++;
    }

    printf("\nresults: %d passed, %d failed\n", passed, failed);
    return (failed > 0) ? 1 : 0;
}
