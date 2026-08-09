/* chaotic_pendulum.c
 *
 * butterfly effect simulation using the logistic map (r=4).
 * demonstrates chaotic divergence caused by 64-bit rounding errors. */

#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_math.h"
#include "../include/simd_f128_io.h"

void run_chaos_simulation() {
    printf("--- chaotic pendulum (butterfly effect) simulation ---\n\n");

    /* initial conditions */
    double d_val = 0.1;
    simd_f128 f_val = simd_f128_from_double(0.1);

    /* logistic map: x_new = 4 * x * (1 - x) */
    const int iterations = 60;

    printf("iterating logistic map (r=4) for %d steps...\n\n", iterations);

    for (int i = 0; i < iterations; i++) {
        /* double precision step */
        d_val = 4.0 * d_val * (1.0 - d_val);

        /* simd-f128 precision step */
        simd_f128 one = simd_f128_from_double(1.0);
        simd_f128 four = simd_f128_from_double(4.0);

        simd_f128 one_minus_x = simd_f128_sub(one, f_val);
        simd_f128 x_times_one_minus_x = simd_f128_mul(f_val, one_minus_x);
        f_val = simd_f128_mul(four, x_times_one_minus_x);
    }

    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), f_val);

    printf("final state (double)    : %.16f\n", d_val);
    printf("final state (simd-f128) : %s\n\n", buf);
    printf("notice how the double precision result has completely diverged\n");
    printf("from the true mathematical trajectory due to chaotic amplification\n");
    printf("of 64-bit rounding errors!\n");
}

int main() {
    run_chaos_simulation();
    return 0;
}
