#define SIMD_F128_IMPLEMENTATION
#include "include/simd_f128.h"
#include "include/simd_f128_math.h"
#include "include/simd_f128_io.h"
#include <stdio.h>

int main() {
    simd_f128 x = simd_f128_from_double(709.5);
    simd_f128 y = simd_f128_exp(x);
    simd_f128_print(y);
    
    simd_f128 x2 = simd_f128_from_double(709.1);
    simd_f128 y2 = simd_f128_exp(x2);
    simd_f128_print(y2);
    return 0;
}
