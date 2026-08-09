/* test_weird_cases.c
 *
 * unit tests for mathematical edge cases.
 * validates infinites, nans, and catastrophic cancellations.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_math.h"
#include "../include/simd_f128_io.h"

int main() {
    printf("=======================================\n");
    printf("    SIMD-F128 WEIRD CASES TESTING\n");
    printf("=======================================\n\n");

    char buf[256];

    // [TEST CASE] extreme exp bounds
    // verifies exp(x) boundaries against float limits.
    printf("--- [1] Extreme exp bounds ---\n");
    simd_f128 val_exp_high = simd_f128_from_double(709.78271289338399);
    simd_f128 val_exp_low = simd_f128_from_double(-745.13321910194110);
    simd_f128 exp_max = simd_f128_exp(val_exp_high);
    simd_f128 exp_min = simd_f128_exp(val_exp_low);
    simd_f128_to_string(buf, sizeof(buf), exp_max);
    printf("exp(709.78271...) = %s (expect near 1.79e308)\n", buf);
    simd_f128_to_string(buf, sizeof(buf), exp_min);
    printf("exp(-745.1332...) = %s (expect 0.0 or subnormal)\n", buf);

    // [TEST CASE] weird pow cases
    // verifies negative base with fractional exponents.
    printf("\n--- [2] Weird pow cases ---\n");
    simd_f128 neg_base = simd_f128_from_double(-2.5);
    simd_f128 int_exp = simd_f128_from_double(3.0);
    simd_f128 frac_exp = simd_f128_from_double(3.5);

    simd_f128 pow1 = simd_f128_pow(neg_base, int_exp); // -2.5^3 = -15.625
    simd_f128 pow2 = simd_f128_pow(neg_base, frac_exp); // -2.5^3.5 = NaN

    simd_f128_to_string(buf, sizeof(buf), pow1);
    printf("pow(-2.5, 3.0)   = %s (expect -15.625)\n", buf);
    simd_f128_to_string(buf, sizeof(buf), pow2);
    printf("pow(-2.5, 3.5)   = %s (expect NaN)\n", buf);

    // [TEST CASE] pow with zeros
    // checks 0^0 and 0^-1 special values.
    simd_f128 zero = simd_f128_from_double(0.0);
    simd_f128 neg_one = simd_f128_from_double(-1.0);

    simd_f128 pow3 = simd_f128_pow(zero, zero);
    simd_f128 pow4 = simd_f128_pow(zero, neg_one); // 0^-1 = inf

    simd_f128_to_string(buf, sizeof(buf), pow3);
    printf("pow(0.0, 0.0)    = %s (expect 1.0)\n", buf);
    simd_f128_to_string(buf, sizeof(buf), pow4);
    printf("pow(0.0, -1.0)   = %s (expect inf)\n", buf);

    // [TEST CASE] catastrophic cancellation
    // checks precision loss on very close floating values.
    printf("\n--- [4] Catastrophic Cancellation Check ---\n");
    simd_f128 a = simd_f128_from_string("1.0000000000000000000000000000001");
    simd_f128 b = simd_f128_from_double(1.0);
    simd_f128 c = simd_f128_sub(a, b);
    simd_f128_to_string(buf, sizeof(buf), c);
    printf("1.0...01 - 1.0   = %s\n", buf);

    // [TEST CASE] huge bounds trig
    // verifies trigonometric boundaries and period reduction.
    printf("\n--- [5] Trig huge bounds ---\n");
    simd_f128 huge_val = simd_f128_from_double(1e100);
    simd_f128 sin_huge = simd_f128_sin(huge_val);
    simd_f128_to_string(buf, sizeof(buf), sin_huge);
    printf("sin(1e100)       = %s (should be within [-1, 1] without crashing)\n", buf);

    // [TEST CASE] atan2 signs
    // verifies +/- zero distinctions for angle quadrants.
    printf("\n--- [6] atan2 signs ---\n");
    simd_f128 neg_zero = simd_f128_from_double(-0.0);
    simd_f128 atan2_1 = simd_f128_atan2(zero, neg_zero); // atan2(0, -0) = PI
    simd_f128 atan2_2 = simd_f128_atan2(neg_zero, neg_zero); // atan2(-0, -0) = -PI

    simd_f128_to_string(buf, sizeof(buf), atan2_1);
    printf("atan2(0.0, -0.0) = %s\n", buf);
    simd_f128_to_string(buf, sizeof(buf), atan2_2);
    printf("atan2(-0.0, -0.0)= %s\n", buf);

    // [TEST CASE] division by infinity
    // verifies convergence to 0.0 without nan corruption.
    printf("\n--- [7] division by infinity ---\n");
    simd_f128 one_val = simd_f128_from_double(1.5);
    simd_f128 inf_val = simd_f128_from_double(INFINITY);
    simd_f128 div_inf = simd_f128_div(one_val, inf_val);
    simd_f128_to_string(buf, sizeof(buf), div_inf);
    printf("1.5 / infinity   = %s (expect 0.0)\n", buf);

    // [TEST CASE] abs negative zero
    // verifies that signbit is cleared properly on -0.0.
    printf("\n--- [8] abs negative zero ---\n");
    simd_f128 abs_neg_zero = simd_f128_abs(neg_zero);
    double abs_hi, abs_lo;
    simd_f128_extract(abs_neg_zero, &abs_hi, &abs_lo);
    printf("abs(-0.0) hi     = %f (expect 0.0, not -0.0)\n", abs_hi);
    printf("abs(-0.0) signbit= %d (expect 0)\n", signbit(abs_hi));

    // [TEST CASE] asin/acos precision
    // validates endpoint precision around +/- 1.0.
    printf("\n--- [9] asin/acos endpoint precision ---\n");
    simd_f128 near_one = simd_f128_sub(simd_f128_from_double(1.0), simd_f128_from_double(1e-15));
    simd_f128 asin_near_one = simd_f128_asin(near_one);
    simd_f128_to_string(buf, sizeof(buf), asin_near_one);
    printf("asin(1 - 1e-15)  = %s\n", buf);
    simd_f128 acos_near_one = simd_f128_acos(near_one);
    simd_f128_to_string(buf, sizeof(buf), acos_near_one);
    printf("acos(1 - 1e-15)  = %s\n", buf);

    // 10. atan range reduction check
    printf("\n--- [10] atan range reduction ---\n");
    simd_f128 huge_atan_val = simd_f128_from_double(1e15);
    simd_f128 atan_huge = simd_f128_atan(huge_atan_val);
    simd_f128_to_string(buf, sizeof(buf), atan_huge);
    printf("atan(1e15)       = %s\n", buf);

    // 11. parser subnormal string check
    printf("\n--- [11] parser subnormal scientific notation ---\n");
    simd_f128 subnormal_parsed = simd_f128_from_string("1.5e-320");
    simd_f128_to_string(buf, sizeof(buf), subnormal_parsed);
    printf("1.5e-320 parsed  = %s (expect non-nan, non-zero subnormal)\n", buf);

    printf("\n--- All tests executed smoothly without exceptions! ---\n");
    return 0;
}
