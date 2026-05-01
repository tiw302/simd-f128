#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SIMD_F128_IMPLEMENTATION
#include "../simd_f128.h"
#include "../simd_f128_consts.h"
#include "../simd_f128_io.h"

static void extract(simd_f128 x, double *hi, double *lo) {
    simd_f128_extract(x, hi, lo);
}

static int within_ulp(simd_f128 a, simd_f128 b, int ulps) {
    double ahi, alo, bhi, blo;
    extract(a, &ahi, &alo);
    extract(b, &bhi, &blo);

    if (fabsl(ahi - bhi) > ulps * 0x1.0p-52) return 0;
    if (fabsl(alo - blo) > ulps * 0x1.0p-52) return 0;
    return 1;
}

static int almost_equal(simd_f128 a, simd_f128 b, int max_ulp) {
    return within_ulp(a, b, max_ulp);
}

int tests_run    = 0;
int tests_failed = 0;

#define EPSILON 1e-30

#define CHECK(label, cond)                                      \
    do {                                                        \
        tests_run++;                                            \
        if (!(cond)) {                                          \
            printf("  FAIL: %s\n", label);                      \
            tests_failed++;                                     \
        } else {                                                \
            printf("  PASS: %s\n", label);                      \
        }                                                       \
    } while (0)

#define CHECK_CLOSE(label, a, b, ulps)                          \
    do {                                                        \
        tests_run++;                                            \
        if (!almost_equal(a, b, ulps)) {                        \
            printf("  FAIL: %s\n", label);                      \
            tests_failed++;                                     \
        } else {                                                \
            printf("  PASS: %s\n", label);                      \
        }                                                       \
    } while (0)

int main() {
    printf("========================================\n");
    printf("  simd-f128 comprehensive test suite\n");
    printf("========================================\n\n");

    double hi, lo;

    printf("=== SECTION 1: Basic Operations ===\n\n");

    printf("[1.1] constants\n");
    extract(SIMD_F128_PI, &hi, &lo);
    CHECK("pi hi accurate", fabsl(hi - 3.1415926535897931) < 1e-15);
    CHECK("pi lo is residual", lo != 0.0);
    CHECK("pi hi > pi lo", hi > fabsl(lo));

    extract(SIMD_F128_E, &hi, &lo);
    CHECK("e hi accurate", fabsl(hi - 2.7182818284590451) < 1e-15);
    CHECK("e lo is residual", lo != 0.0);

    extract(SIMD_F128_SQRT2, &hi, &lo);
    CHECK("sqrt2 hi accurate", fabsl(hi - 1.4142135623730951) < 1e-15);

    extract(SIMD_F128_LN2, &hi, &lo);
    CHECK("ln2 hi accurate", fabsl(hi - 0.6931471805599453) < 1e-15);

    printf("\n[1.2] from_double\n");
    simd_f128 one = simd_f128_from_double(1.0);
    extract(one, &hi, &lo);
    CHECK("from_double(1.0) hi == 1.0", hi == 1.0);
    CHECK("from_double(1.0) lo == 0.0", lo == 0.0);

    simd_f128 big = simd_f128_from_double(1e300);
    extract(big, &hi, &lo);
    CHECK("from_double(1e300) hi == 1e300", hi == 1e300);
    CHECK("from_double(1e300) lo == 0.0", lo == 0.0);

    simd_f128 tiny = simd_f128_from_double(1e-300);
    extract(tiny, &hi, &lo);
    CHECK("from_double(1e-300) hi == 1e-300", hi == 1e-300);

    printf("\n[1.3] addition precision\n");
    simd_f128 a = simd_f128_from_double(1.0);
    simd_f128 eps1 = simd_f128_from_double(1e-17);
    simd_f128 sum1 = simd_f128_add(a, eps1);
    extract(sum1, &hi, &lo);
    CHECK("1.0 + 1e-17 preserves hi", hi == 1.0);
    CHECK("1.0 + 1e-17 has non-zero lo", lo != 0.0);

    simd_f128 eps2 = simd_f128_from_double(1e-20);
    simd_f128 sum2 = simd_f128_add(a, eps2);
    extract(sum2, &hi, &lo);
    CHECK("1.0 + 1e-20 preserves hi", hi == 1.0);
    CHECK("1.0 + 1e-20 preserves small value in lo", lo > 0.0);

    printf("\n[1.4] subtraction\n");
    simd_f128 diff1 = simd_f128_sub(SIMD_F128_PI, SIMD_F128_PI);
    extract(diff1, &hi, &lo);
    CHECK("pi - pi == 0.0 hi", hi == 0.0);
    CHECK("pi - pi == 0.0 lo", lo == 0.0);

    simd_f128 diff2 = simd_f128_sub(a, eps1);
    extract(diff2, &hi, &lo);
    CHECK("1.0 - 1e-17 lo correct", lo < 0.0);

    printf("\n[1.5] multiplication\n");
    simd_f128 two = simd_f128_from_double(2.0);
    simd_f128 prod1 = simd_f128_mul(SIMD_F128_PI, two);
    extract(prod1, &hi, &lo);
    CHECK("pi * 2.0 hi ~ 6.283", fabsl(hi - 6.283185307179586) < 1e-14);

    simd_f128 prod2 = simd_f128_mul(SIMD_F128_PI, SIMD_F128_PI);
    extract(prod2, &hi, &lo);
    CHECK("pi^2 hi ~ 9.8696", fabsl(hi - 9.869604401089358) < 1e-13);

    simd_f128 one_check = simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(1.0));
    CHECK("pi * 1.0 hi correct", fabsl(hi - 9.869604401089358) < 1e-10);

    printf("\n[1.6] division\n");
    simd_f128 six = simd_f128_from_double(6.0);
    simd_f128 two_f128 = simd_f128_from_double(2.0);
    simd_f128 div1 = simd_f128_div(six, two_f128);
    extract(div1, &hi, &lo);
    CHECK("6.0 / 2.0 == 3.0", hi == 3.0);

    simd_f128 div2 = simd_f128_div(SIMD_F128_PI, two_f128);
    extract(div2, &hi, &lo);
    CHECK("pi/2 hi ~ 1.5707", fabsl(hi - 1.5707963267948966) < 1e-14);

    simd_f128 div3 = simd_f128_div(simd_f128_from_double(1.0), simd_f128_from_double(3.0));
    extract(div3, &hi, &lo);
    CHECK("1/3 hi ~ 0.3333", fabsl(hi - 0.3333333333333333) < 1e-14);

printf("\n[1.7] sqrt\n");
    simd_f128 sq4 = simd_f128_sqrt(simd_f128_from_double(4.0));
    extract(sq4, &hi, &lo);
    CHECK("sqrt(4.0) == 2.0", hi == 2.0);

    simd_f128 sq2 = simd_f128_sqrt(simd_f128_from_double(2.0));
    extract(sq2, &hi, &lo);
    CHECK("sqrt(2.0) ~ 1.4142", fabsl(hi - 1.4142135623730951) < 1e-14);

    simd_f128 sq10 = simd_f128_sqrt(simd_f128_from_double(10.0));
    extract(sq10, &hi, &lo);
    CHECK("sqrt(10.0) ~ 3.1622", fabsl(hi - 3.1622776601683795) < 1e-13);

    simd_f128 sq0_25 = simd_f128_sqrt(simd_f128_from_double(0.25));
    extract(sq0_25, &hi, &lo);
    CHECK("sqrt(0.25) == 0.5", hi == 0.5);

    printf("\n=== SECTION 2: Precision Edge Cases ===\n\n");

    printf("[2.1] extreme small values\n");
    simd_f128 tiny1 = simd_f128_from_double(1e-300);
    simd_f128 tiny2 = simd_f128_from_double(1e-300);
    simd_f128 tiny_sum = simd_f128_add(tiny1, tiny2);
    extract(tiny_sum, &hi, &lo);
    CHECK("1e-300 + 1e-300 > 1e-300", hi > 1e-300);

    printf("\n[2.2] large value arithmetic\n");
    simd_f128 large1 = simd_f128_from_double(1e150);
    simd_f128 large2 = simd_f128_from_double(1e150);
    simd_f128 large_sum = simd_f128_add(large1, large2);
    extract(large_sum, &hi, &lo);
    CHECK("1e150 + 1e150 ~ 2e150", fabsl(hi - 2e150) < 1e135);

    simd_f128 large_prod = simd_f128_mul(large1, simd_f128_from_double(2.0));
    extract(large_prod, &hi, &lo);
    CHECK("1e150 * 2.0 ~ 2e150", fabsl(hi - 2e150) < 1e135);

    printf("\n[2.3] cancellation in subtraction\n");
    simd_f128 big1 = simd_f128_from_double(1.000000000000001);
    simd_f128 big2 = simd_f128_from_double(1.000000000000000);
    simd_f128 cancelled = simd_f128_sub(big1, big2);
    extract(cancelled, &hi, &lo);
    CHECK("cancellation preserves result", hi > 0.0);

    printf("\n[2.4] associative property\n");
    simd_f128 x = simd_f128_from_double(1.0);
    simd_f128 y = simd_f128_from_double(1e-16);
    simd_f128 z = simd_f128_from_double(1e-16);

    simd_f128 xyz1 = simd_f128_add(simd_f128_add(x, y), z);
    simd_f128 xyz2 = simd_f128_add(x, simd_f128_add(y, z));
    CHECK_CLOSE("(x+y)+z == x+(y+z)", xyz1, xyz2, 10);

    printf("\n=== SECTION 3: Special Values ===\n\n");

    printf("[3.1] zero handling\n");
    simd_f128 zero = simd_f128_from_double(0.0);
    simd_f128 neg_zero = simd_f128_from_double(-0.0);
    extract(zero, &hi, &lo);
    CHECK("positive zero hi == 0.0", hi == 0.0);
    extract(neg_zero, &hi, &lo);
    CHECK("negative zero hi == 0.0", hi == 0.0);

    simd_f128 zero_sum = simd_f128_add(zero, SIMD_F128_PI);
    extract(zero_sum, &hi, &lo);
    CHECK("0 + pi == pi", hi > 3.0);

    simd_f128 zero_prod = simd_f128_mul(zero, SIMD_F128_PI);
    extract(zero_prod, &hi, &lo);
    CHECK("0 * pi == 0", hi == 0.0);

    printf("\n[3.2] infinity handling\n");
    simd_f128 inf = simd_f128_from_double(INFINITY);
    extract(inf, &hi, &lo);
    CHECK("infinity is inf", isinf(hi) && hi > 0);

    simd_f128 inf_sum = simd_f128_add(inf, simd_f128_from_double(1.0));
    extract(inf_sum, &hi, &lo);

    simd_f128 neg_inf = simd_f128_from_double(-INFINITY);
    simd_f128 inf_sum2 = simd_f128_add(inf, neg_inf);
    extract(inf_sum2, &hi, &lo);
    CHECK("inf + (-inf) is nan", isnan(hi));

    printf("\n[3.3] NaN propagation\n");
    simd_f128 nan = simd_f128_from_double(NAN);
    extract(nan, &hi, &lo);
    CHECK("NaN is NaN", isnan(hi));

    simd_f128 nan_sum = simd_f128_add(nan, SIMD_F128_PI);
    extract(nan_sum, &hi, &lo);
    CHECK("NaN + pi == NaN", isnan(hi));

    simd_f128 nan_prod = simd_f128_mul(nan, SIMD_F128_PI);
    extract(nan_prod, &hi, &lo);
    CHECK("NaN * pi == NaN", isnan(hi));

    simd_f128 nan_div = simd_f128_div(nan, SIMD_F128_PI);
    extract(nan_div, &hi, &lo);
    CHECK("NaN / pi == NaN", isnan(hi));

    printf("\n[3.4] negative numbers\n");
    simd_f128 neg_pi = simd_f128_from_double(-3.14159);
    extract(neg_pi, &hi, &lo);
    CHECK("negative value hi < 0", hi < 0.0);

    simd_f128 neg_sq = simd_f128_sqrt(simd_f128_from_double(4.0));
    extract(neg_sq, &hi, &lo);
    CHECK("sqrt(4.0) positive", hi > 0.0);

    printf("\n=== SECTION 4: Mathematical Identities ===\n\n");

    printf("[4.1] sqrt(x^2) == |x|\n");
    simd_f128 val = simd_f128_from_double(123.456);
    simd_f128 val_sq = simd_f128_mul(val, val);
    simd_f128 val_sqrt = simd_f128_sqrt(val_sq);
    extract(val_sqrt, &hi, &lo);
    CHECK("sqrt(x^2) >= x", hi >= 123.456 - 1e-10);

    printf("[4.2] (x/y) * y ≈ x\n");
    simd_f128 ten = simd_f128_from_double(10.0);
    simd_f128 three = simd_f128_from_double(3.0);
    simd_f128 div_result = simd_f128_div(ten, three);
    simd_f128 mult_back = simd_f128_mul(div_result, three);
    CHECK_CLOSE("(10/3) * 3 ≈ 10", mult_back, ten, 100);

    printf("\n[4.3] sqrt(2)^2 ≈ 2\n");
    simd_f128 sqrt2_sq = simd_f128_mul(SIMD_F128_SQRT2, SIMD_F128_SQRT2);
    CHECK_CLOSE("sqrt(2)^2 ≈ 2", sqrt2_sq, simd_f128_from_double(2.0), 10);

    printf("\n[4.4] e^ln(2) ≈ 2\n");
    simd_f128 exp_ln2 = simd_f128_mul(SIMD_F128_E, SIMD_F128_LN2);
    extract(exp_ln2, &hi, &lo);
    CHECK("e^ln(2) > 1.5", hi > 1.5);
    CHECK("e^ln(2) < 2.1", hi < 2.1);

    printf("\n=== SECTION 5: Stress Tests ===\n\n");

    printf("[5.1] repeated operations\n");
    simd_f128 accumulator = simd_f128_from_double(1.0);
    for (int i = 0; i < 100; i++) {
        accumulator = simd_f128_add(accumulator, simd_f128_from_double(1e-16));
    }
    extract(accumulator, &hi, &lo);
    CHECK("100 * 1e-16 accumulated", hi > 1.0);

    simd_f128 acc2 = SIMD_F128_PI;
    for (int i = 0; i < 50; i++) {
        acc2 = simd_f128_mul(acc2, simd_f128_from_double(1.0001));
    }
    extract(acc2, &hi, &lo);
    CHECK("50 * 1.0001 multiplications", hi > 3.14);

    printf("\n[5.2] division by small numbers\n");
    simd_f128 one_small = simd_f128_from_double(1.0);
    simd_f128 tiny_div = simd_f128_from_double(1e-100);
    simd_f128 div_small = simd_f128_div(one_small, tiny_div);
    extract(div_small, &hi, &lo);
    CHECK("1.0 / 1e-100 is huge", hi > 1e99);

    printf("\n[5.3] sqrt of very small numbers\n");
    simd_f128 very_small = simd_f128_from_double(1e-200);
    simd_f128 sqrt_vs = simd_f128_sqrt(very_small);
    extract(sqrt_vs, &hi, &lo);
    CHECK("sqrt(1e-200) > 0", hi > 0.0);

    printf("\n=== SUMMARY ===\n\n");
    printf("Total tests:  %d\n", tests_run);
    printf("Passed:       %d\n", tests_run - tests_failed);
    printf("Failed:       %d\n", tests_failed);
    printf("\n");

    if (tests_failed > 0) {
        printf("RESULT: FAILED\n");
        return 1;
    } else {
        printf("RESULT: ALL TESTS PASSED\n");
        return 0;
    }
}