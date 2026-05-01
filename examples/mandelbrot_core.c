#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../simd_f128.h"
#include "../simd_f128_io.h"

int main() {
    printf("--- mandelbrot core loop (128-bit precision) ---\n\n");

    /*
     * Deep-zoom coordinate that requires more than 64-bit precision
     * to produce a correct iteration count.
     */
    simd_f128 cx = simd_f128_from_double(-0.7436438870371587);
    simd_f128 cy = simd_f128_from_double( 0.1318259042053119);

    simd_f128 zx = simd_f128_from_double(0.0);
    simd_f128 zy = simd_f128_from_double(0.0);

    simd_f128 two      = simd_f128_from_double(2.0);
    simd_f128 escape_r = simd_f128_from_double(4.0); /* |z|^2 > 4 => escaped */

    const int max_iter = 500;
    int iter = 0;

    for (iter = 0; iter < max_iter; iter++) {
        simd_f128 zx2 = simd_f128_mul(zx, zx);
        simd_f128 zy2 = simd_f128_mul(zy, zy);

        /* escape check: zx^2 + zy^2 > 4 */
        simd_f128 mag2 = simd_f128_add(zx2, zy2);

        double mag2_hi, mag2_lo;
        simd_f128_extract(mag2, &mag2_hi, &mag2_lo);
        if (mag2_hi > 4.0) break;

        /* z = z^2 + c */
        simd_f128 new_zx = simd_f128_add(simd_f128_sub(zx2, zy2), cx);
        simd_f128 new_zy = simd_f128_add(simd_f128_mul(two, simd_f128_mul(zx, zy)), cy);

        zx = new_zx;
        zy = new_zy;
    }

    if (iter < max_iter) {
        printf("escaped at iteration %d (point is outside the Mandelbrot set)\n", iter);
    } else {
        printf("did not escape after %d iterations (point is inside the Mandelbrot set)\n", max_iter);
    }

    printf("\nfinal |z| components:\n");
    printf("  zx = "); simd_f128_print(zx);
    printf("  zy = "); simd_f128_print(zy);

    return 0;
}