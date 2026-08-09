/* basic_arithmetic.c
 *
 * basic arithmetic operations using 128-bit constants. */

#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_consts.h"
#include "../include/simd_f128_io.h"
static void run_arithmetic_demo(void) {
    if (printf("--- simd-f128 basic arithmetic ---\n\n") < 0) {
        return;
    }

    /* constants are exact to 31 digits */
    const simd_f128 val_pi = SIMD_F128_PI;
    const simd_f128 val_e = SIMD_F128_E;

    simd_f128 sum = simd_f128_add(val_pi, val_e);
    simd_f128 diff = simd_f128_sub(val_pi, val_e);
    simd_f128 prod = simd_f128_mul(val_pi, val_e);

    printf("pi + e  = ");
    simd_f128_print(sum);

    printf("pi - e  = ");
    simd_f128_print(diff);

    printf("pi * e  = ");
    simd_f128_print(prod);
}

int main(void) {
    run_arithmetic_demo();
    return 0;
}
