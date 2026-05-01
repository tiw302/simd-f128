#ifndef SIMD_F128_CONSTS_H
#define SIMD_F128_CONSTS_H

#include "simd_f128.h"

/*
    ██████  ██████  ███    ██ ███████ ████████ ███████ 
    ██      ██  ██  ████   ██ ██         ██    ██      
    ██      ██  ██  ██ ██  ██ ███████    ██    ███████ 
    ██      ██  ██  ██  ██ ██      ██    ██         ██ 
    ██████  ██████  ██   ████ ███████    ██    ███████ 
*/

/*
 * high-precision mathematical constants (approx. 31 decimal digits)
 * calculated as hi (64-bit ieee) + lo (64-bit ieee residual error).
 */

/* 
 * Compile-time constants cannot use intrinsic functions like wasm_f64x2_const
 * in standard C global scope if the compiler isn't built to fold them.
 * To ensure 100% cross-platform compilation (including WASM), we declare them
 * as generic scalar arrays and type-cast them as needed in a helper.
 */

static const double _SIMD_F128_PI_VAL[2]    = { 1.2246467991473532e-16, 3.1415926535897931 };
static const double _SIMD_F128_E_VAL[2]     = { 1.4456468917292502e-16, 2.7182818284590451 };
static const double _SIMD_F128_SQRT2_VAL[2] = { -9.6672933134529135e-17, 1.4142135623730951 };
static const double _SIMD_F128_LN2_VAL[2]   = { 2.3190468138462996e-17, 0.69314718055994529 };

#if defined(SIMD_F128_USE_AVX2) || defined(SIMD_F128_USE_NEON) || defined(SIMD_F128_USE_WASM)
    /* For vector types, we trick the compiler by reading the memory directly */
    #define SIMD_F128_PI    (*(const simd_f128*)_SIMD_F128_PI_VAL)
    #define SIMD_F128_E     (*(const simd_f128*)_SIMD_F128_E_VAL)
    #define SIMD_F128_SQRT2 (*(const simd_f128*)_SIMD_F128_SQRT2_VAL)
    #define SIMD_F128_LN2   (*(const simd_f128*)_SIMD_F128_LN2_VAL)
#else
    /* For scalar struct {hi, lo}, index 1 is hi, index 0 is lo */
    #define SIMD_F128_PI    (*(const simd_f128*)((const double[]){3.1415926535897931, 1.2246467991473532e-16}))
    #define SIMD_F128_E     (*(const simd_f128*)((const double[]){2.7182818284590451, 1.4456468917292502e-16}))
    #define SIMD_F128_SQRT2 (*(const simd_f128*)((const double[]){1.4142135623730951, -9.6672933134529135e-17}))
    #define SIMD_F128_LN2   (*(const simd_f128*)((const double[]){0.69314718055994529, 2.3190468138462996e-17}))
#endif

#endif /* SIMD_F128_CONSTS_H */
