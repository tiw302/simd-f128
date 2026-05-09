// updated 2026-05-09

#ifndef SIMD_F128_CONSTS_H
#define SIMD_F128_CONSTS_H

#include "simd_f128.h"
#include <string.h>

/*
    simd_f128_consts.h -- high-precision mathematical constants.

    provides pre-computed double-double constants with ~106-bit mantissa
    (approximately 31 decimal digits of precision).
*/

//  ██████  ██████  ███    ██ ███████ ████████ ███████ 
// ██      ██    ██ ████   ██ ██         ██    ██      
// ██      ██    ██ ██ ██  ██ ███████    ██    ███████ 
// ██      ██    ██ ██  ██ ██      ██    ██         ██ 
//  ██████  ██████  ██   ████ ███████    ██    ███████ 
//
// >>constants storage

/*
 * these constants are stored as hi + lo pairs.
 * we keep them in raw double arrays so they're easy to load across 
 * different architectures without worrying about struct layout.
 */
static const double _simd_f128_pi_raw[2]    = { 3.1415926535897931, 1.2246467991473532e-16 };
static const double _simd_f128_e_raw[2]     = { 2.7182818284590451, 1.4456468917292502e-16 };
static const double _simd_f128_sqrt2_raw[2] = { 1.4142135623730951, -9.6672933134529135e-17 };
static const double _simd_f128_ln2_raw[2]   = { 0.69314718055994529, 2.3190468138462996e-17 };

/*
 * helper to load the raw doubles into our simd_f128 type.
 * we use memcpy to avoid aliasing issues, but don't worry, 
 * the compiler is smart enough to turn this into a direct load.
 */
static inline simd_f128 _simd_f128_from_raw(const double *raw) {
    simd_f128 result;
#if defined(SIMD_F128_USE_SCALAR)
    result.hi = raw[0];
    result.lo = raw[1];
#else
    memcpy(&result, raw, sizeof(result));
#endif
    return result;
}

#define SIMD_F128_PI    _simd_f128_from_raw(_simd_f128_pi_raw)
#define SIMD_F128_E     _simd_f128_from_raw(_simd_f128_e_raw)
#define SIMD_F128_SQRT2 _simd_f128_from_raw(_simd_f128_sqrt2_raw)
#define SIMD_F128_LN2   _simd_f128_from_raw(_simd_f128_ln2_raw)

#endif /* SIMD_F128_CONSTS_H */
