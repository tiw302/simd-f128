#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../simd_f128.h"
#include "../simd_f128_io.h"

int main() {
    printf("--- precision comparison: double vs simd-f128 ---\n\n");

    /*
     * Adding 1e-17 to 1.0 exceeds the resolution of a 64-bit double.
     * The small value is silently dropped, leaving the result unchanged.
     */
    double big_d   = 1.0;
    double small_d = 1e-17;
    double result_d = big_d + small_d;

    printf("[double]  1.0 + 1e-17 = %.20f\n", result_d);
    printf("          precision lost: %s\n\n", result_d == 1.0 ? "yes" : "no");

    /*
     * simd-f128 stores the residual in the lo component.
     * The small value is preserved and participates in subsequent operations.
     */
    simd_f128 big_f   = simd_f128_from_double(1.0);
    simd_f128 small_f = simd_f128_from_double(1e-17);
    simd_f128 result_f = simd_f128_add(big_f, small_f);

    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), result_f);
    printf("[simd-f128] 1.0 + 1e-17 = %s\n", buf);
    printf("          precision lost: no\n");

    return 0;
}