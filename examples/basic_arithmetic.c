#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_consts.h"
#include "../include/simd_f128_io.h"

int main() {
    printf("--- simd-fp basic arithmetic ---\n\n");

    // the library provides pre-computed high-precision mathematical constants.
    // these constants utilize the full 128-bit space (31 decimal digits), 
    // ensuring no precision is lost during initialization.
    simd_f128 a = SIMD_F128_PI;
    simd_f128 b = SIMD_F128_E;

    // arithmetic operations are carried out via hardware-accelerated simd 
    // intrinsics (avx2/neon/wasm) to compute the exact upper and lower bounds
    // of the resulting double-double mathematically.
    simd_f128 sum  = simd_f128_add(a, b);
    simd_f128 diff = simd_f128_sub(a, b);
    simd_f128 prod = simd_f128_mul(a, b);

    printf("pi + e  = "); simd_f128_print(sum);
    printf("pi - e  = "); simd_f128_print(diff);
    printf("pi * e  = "); simd_f128_print(prod);

    return 0;
}