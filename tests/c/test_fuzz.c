/* test_fuzz.c
 *
 * fuzz testing against __float128 reference.
 * validates precision limits with randomized inputs.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __GNUC__
#include <quadmath.h>
#endif

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"

// generate random double in a reasonable range
double rand_double() {
    double r = (double)rand() / (double)RAND_MAX;
    int exp = (rand() % 100) - 50;  // -50 to 50
    return r * pow(10.0, exp);
}

int main() {
#ifndef __GNUC__
    printf("fuzz testing requires gcc __float128 support.\n");
    return 0;
#else
    srand(time(NULL));
    int num_tests = 100000;
    int failures = 0;

    printf("running %d fuzz tests against __float128...\n", num_tests);

    for (int i = 0; i < num_tests; i++) {
        double a_hi = rand_double();
        double a_lo = a_hi * 1e-16;  // rough estimate of low part
        double b_hi = rand_double();
        double b_lo = b_hi * 1e-16;

        simd_f128 sa = simd_f128_from_hi_lo(a_hi, a_lo);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi, b_lo);

        __float128 qa = (__float128)a_hi + (__float128)a_lo;
        __float128 qb = (__float128)b_hi + (__float128)b_lo;

        // [TEST CASE] add
        // verifies addition against __float128 reference implementation.
        simd_f128 s_add = simd_f128_add(sa, sb);
        __float128 q_add = qa + qb;

        double s_add_hi, s_add_lo;
        simd_f128_extract(s_add, &s_add_hi, &s_add_lo);
        __float128 s_add_q = (__float128)s_add_hi + (__float128)s_add_lo;

        // verify relative error
        __float128 diff = fabsq(s_add_q - q_add);
        __float128 max_val = fabsq(q_add) > 1e-30 ? fabsq(q_add) : 1e-30;
        if (diff / max_val > 1e-30) {  // ~ 30 digits precision limit
            // check if both are infinity/nan
            if (isinf(s_add_hi) && isinfq(q_add)) continue;
            if (isnan(s_add_hi) && isnanq(q_add)) continue;

            failures++;
            if (failures < 10) {
                printf("fuzz failure on add: diff=%e\n", (double)diff);
            }
        }
    }

    if (failures == 0) {
        printf("fuzz tests passed!\n");
        return 0;
    } else {
        printf("fuzz tests failed with %d errors.\n", failures);
        return 1;
    }
#endif
}
