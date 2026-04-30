#include <stdio.h>
#include <math.h>

#define SIMD_F128_IMPLEMENTATION
#include "../simd_f128.h"
#include "../simd_f128_consts.h"
#include "../simd_f128_io.h"

/* extract hi/lo across all backends */
static void extract(simd_f128 x, double *hi, double *lo) {
#if defined(SIMD_F128_USE_AVX2) || defined(SIMD_F128_USE_WASM) || defined(SIMD_F128_USE_NEON)
    *hi = x.d[1];
    *lo = x.d[0];
#else
    *hi = x.hi;
    *lo = x.lo;
#endif
}

int tests_run    = 0;
int tests_failed = 0;

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

int main() {
    printf("running simd-f128 tests...\n\n");

    double hi, lo;

    /* --- test 1: constant sanity --- */
    printf("[1] constants\n");
    extract(SIMD_F128_PI, &hi, &lo);
    CHECK("pi hi ~ 3.14159", fabs(hi - 3.141592653589793) < 1e-15);
    CHECK("pi lo is non-zero residual", lo != 0.0);

    /* --- test 2: addition precision --- */
    printf("\n[2] addition precision\n");
    simd_f128 a   = simd_f128_from_double(1.0);
    simd_f128 eps = simd_f128_from_double(1e-20);
    simd_f128 res = simd_f128_add(a, eps);
    extract(res, &hi, &lo);
    CHECK("1.0 + 1e-20: hi == 1.0", hi == 1.0);
    CHECK("1.0 + 1e-20: lo preserves small value", lo > 0.0);

    /* --- test 3: subtraction --- */
    printf("\n[3] subtraction\n");
    simd_f128 diff = simd_f128_sub(SIMD_F128_PI, SIMD_F128_PI);
    extract(diff, &hi, &lo);
    CHECK("pi - pi: hi == 0.0", hi == 0.0);
    CHECK("pi - pi: lo == 0.0", lo == 0.0);

    /* --- test 4: multiplication identity --- */
    printf("\n[4] multiplication\n");
    simd_f128 one  = simd_f128_from_double(1.0);
    simd_f128 prod = simd_f128_mul(SIMD_F128_PI, one);
    extract(prod, &hi, &lo);
    double pi_hi, pi_lo;
    extract(SIMD_F128_PI, &pi_hi, &pi_lo);
    CHECK("pi * 1.0: hi unchanged", hi == pi_hi);
    CHECK("pi * 1.0: lo unchanged", lo == pi_lo);

    /* --- summary --- */
    printf("\n%d/%d tests passed", tests_run - tests_failed, tests_run);
    if (tests_failed > 0) {
        printf(" (%d failed)\n", tests_failed);
        return 1;
    }
    printf("\n");
    return 0;
}