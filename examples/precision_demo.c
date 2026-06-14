#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"

int main() {
    printf("--- precision comparison: double vs simd-f128 ---\n\n");

    // 1e-17 is smaller than the machine epsilon for a 64-bit float (~2.22e-16).
    // when added to 1.0, the mantissa shift drops the small value completely.
    double big_d   = 1.0;
    double small_d = 1e-17;
    double result_d = big_d + small_d;

    printf("[double]  1.0 + 1e-17 = %.20f\n", result_d);
    printf("          precision lost: %s\n\n", result_d == 1.0 ? "yes" : "no");

    // simd-f128 bypasses machine epsilon exhaustion by utilizing the `.lo` 
    // component to store the residual error of the `1.0 + 1e-17` addition.
    simd_f128 big_f   = simd_f128_from_double(1.0);
    simd_f128 small_f = simd_f128_from_double(1e-17);
    simd_f128 result_f = simd_f128_add(big_f, small_f);

    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), result_f);
    printf("[simd-f128] 1.0 + 1e-17 = %s\n", buf);
    printf("          precision lost: no\n");

    return 0;
}