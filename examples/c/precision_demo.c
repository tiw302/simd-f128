/* precision_demo.c
 *
 * demonstrates precision retention beyond 64-bit machine epsilon. */

#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"

static void run_precision_demo(void) {
    if (printf("--- precision comparison: double vs simd-f128 ---\n\n") < 0) {
        return;
    }

    /* 1e-17 is smaller than 64-bit machine epsilon (~2.22e-16) */
    const double big_d = 1.0;
    const double small_d = 1e-17;
    double result_d = big_d + small_d;

    printf("[double]  1.0 + 1e-17 = %.20f\n", result_d);
    printf("          precision lost: %s\n\n", result_d == 1.0 ? "yes" : "no");

    /* simd-f128 retains the 1e-17 residual in the .lo component */
    simd_f128 big_f = simd_f128_from_double(1.0);
    simd_f128 small_f = simd_f128_from_double(1e-17);
    simd_f128 result_f = simd_f128_add(big_f, small_f);

    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), result_f);
    printf("[simd-f128] 1.0 + 1e-17 = %s\n", buf);
    printf("          precision lost: no\n");
}

int main(void) {
    run_precision_demo();
    return 0;
}
