#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../simd_f128.h"
#include "../simd_f128_consts.h"
#include "../simd_f128_io.h"

int main() {
    printf("--- simd-f128 basic arithmetic ---\n\n");

    simd_f128 a = SIMD_F128_PI;   /* pi  ~ 3.14159265358979323846... */
    simd_f128 b = SIMD_F128_E;    /* e   ~ 2.71828182845904523536... */

    simd_f128 sum  = simd_f128_add(a, b);
    simd_f128 diff = simd_f128_sub(a, b);
    simd_f128 prod = simd_f128_mul(a, b);

    printf("pi + e  = "); simd_f128_print(sum);
    printf("pi - e  = "); simd_f128_print(diff);
    printf("pi * e  = "); simd_f128_print(prod);

    return 0;
}