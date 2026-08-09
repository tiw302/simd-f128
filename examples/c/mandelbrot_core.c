/* mandelbrot_core.c
 *
 * deep zoom mandelbrot iteration demonstrating extreme precision. */

#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"

static void run_mandelbrot_demo(void) {
    if (printf("--- mandelbrot core loop (128-bit precision) ---\n\n") < 0) {
        return;
    }

    /* target deep zoom coordinates where 64-bit precision fails */
    const simd_f128 cx = simd_f128_from_double(-0.7436438870371587);
    const simd_f128 cy = simd_f128_from_double( 0.1318259042053119);

    simd_f128 zx = simd_f128_from_double(0.0);
    simd_f128 zy = simd_f128_from_double(0.0);

    const int max_iter = 500;
    int iter = 0;

    for (iter = 0; iter < max_iter; iter++) {
        simd_f128 zx2 = simd_f128_mul(zx, zx);
        simd_f128 zy2 = simd_f128_mul(zy, zy);

        /* check escape condition mag^2 > 4.0 */
        simd_f128 mag2 = simd_f128_add(zx2, zy2);

        double mag2_hi, mag2_lo;
        simd_f128_extract(mag2, &mag2_hi, &mag2_lo);
        if (mag2_hi > 4.0) {
            break;
        }

        /* z = z^2 + c */
        simd_f128 new_zx = simd_f128_add(simd_f128_sub(zx2, zy2), cx);

        simd_f128 zx_zy = simd_f128_mul(zx, zy);
        simd_f128 new_zy = simd_f128_add(simd_f128_add(zx_zy, zx_zy), cy);

        zx = new_zx;
        zy = new_zy;
    }

    if (iter < max_iter) {
        printf("escaped at iteration %d (point is outside the Mandelbrot set)\n", iter);
    } else {
        printf("did not escape after %d iterations (point is inside the Mandelbrot set)\n", max_iter);
    }

    printf("\nfinal |z| components:\n");
    printf("  zx = ");
    simd_f128_print(zx);

    printf("  zy = ");
    simd_f128_print(zy);
}

int main(void) {
    run_mandelbrot_demo();
    return 0;
}
