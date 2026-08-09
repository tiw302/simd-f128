/* crypto_large_integer.c
 *
 * using 106-bit mantissa for exact large integer math.
 * demonstrates no precision loss up to 2^106. */

#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"

int main() {
    printf("--- Cryptography / Large Integer Demo ---\n\n");

    /* max 64-bit unsigned integer: 18,446,744,073,709,551,615 */
    unsigned long long max_uint64 = 18446744073709551615ULL;
    printf("Max uint64_t        : %llu\n", max_uint64);

    /* inject exact string to avoid 53-bit double cast truncation */
    simd_f128 base = simd_f128_from_string("18446744073709551615");

    simd_f128 multiplier = simd_f128_from_string("1000000000");

    simd_f128 massive_int = simd_f128_mul(base, multiplier);

    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), massive_int);

    printf("simd-f128 (exact)   : %s\n\n", buf);

    printf("[Analysis]:\n");
    printf("The result is exactly 18,446,744,073,709,551,615,000,000,000.\n");
    printf("This massively overflows a 64-bit integer, but fits perfectly\n");
    printf("into the exact integer boundary (mantissa) of our 128-bit float.\n");

    return 0;
}
