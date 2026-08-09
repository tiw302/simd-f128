/* test_random.c
 *
 * unit tests for simd_f128_random module.
 * validates random number generation and state propagation.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_random.h"
#include "../include/simd_f128_utils.h"

// [TEST CASE] random initialization
// verifies state is populated properly.
int test_random_init() {
    simd_f128_prng state;
    simd_f128_prng_seed(&state, 123456789ULL);
    
    // check that state array is non-zero
    for (int i=0; i<4; i++) {
        if (state.s[i] == 0) return 0;
    }
    return 1;
}

// [TEST CASE] random simd_f128 uniform generation
// verifies random 128-bit float generation properties.
int test_random_simd_f128() {
    simd_f128_prng state;
    simd_f128_prng_seed(&state, 42ULL);
    
    simd_f128 r = simd_f128_prng_uniform(&state);
    double hi, lo;
    simd_f128_extract(r, &hi, &lo);
    
    if (hi < 0.0 || hi >= 1.0) return 0;
    
    // check that it doesn't generate identical sequence immediately
    simd_f128 r2 = simd_f128_prng_uniform(&state);
    double hi2, lo2;
    simd_f128_extract(r2, &hi2, &lo2);
    
    if (hi == hi2 && lo == lo2) return 0;
    
    return 1;
}

int main() {
    int passed = 0;
    int failed = 0;
    
    printf("running simd-f128 random tests...\n");

    if (test_random_init()) {
        printf("  [PASS] test_random_init\n"); passed++;
    } else {
        printf("  [FAIL] test_random_init\n"); failed++;
    }

    if (test_random_simd_f128()) {
        printf("  [PASS] test_random_simd_f128\n"); passed++;
    } else {
        printf("  [FAIL] test_random_simd_f128\n"); failed++;
    }

    printf("\nresults: %d passed, %d failed\n", passed, failed);
    return (failed > 0) ? 1 : 0;
}
