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

#if defined(SIMD_F128_USE_AVX2) || defined(SIMD_F128_USE_WASM) || defined(SIMD_F128_USE_NEON)
    /* vectors: lane 0 = lo, lane 1 = hi — load lo first in memory */
    #define SIMD_F128_MAKE_CONST(h, l) { .d = { (l), (h) } }
#else
    /* scalar struct is defined as { hi, lo } */
    #define SIMD_F128_MAKE_CONST(h, l) { (h), (l) }
#endif

static const simd_f128 SIMD_F128_PI    = SIMD_F128_MAKE_CONST(3.1415926535897931, 1.2246467991473532e-16);
static const simd_f128 SIMD_F128_E     = SIMD_F128_MAKE_CONST(2.7182818284590451, 1.4456468917292502e-16);
static const simd_f128 SIMD_F128_SQRT2 = SIMD_F128_MAKE_CONST(1.4142135623730951, -9.6672933134529135e-17);
static const simd_f128 SIMD_F128_LN2   = SIMD_F128_MAKE_CONST(0.69314718055994529, 2.3190468138462996e-17);

#endif /* SIMD_F128_CONSTS_H */
