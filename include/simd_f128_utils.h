// updated 2026-05-23

#ifndef SIMD_F128_UTILS_H
#define SIMD_F128_UTILS_H

#include "simd_f128.h"

// ██ ██████  
// ██ ██  ██ 
// ██ ██  ██ 
// ██ ██  ██ 
// ██ ██████ 
//
// >>comparison api

#ifdef __cplusplus
extern "C" {
#endif

/*
 * returns -1 if a < b, 1 if a > b, and 0 if a == b.
 * this is the foundation for all other comparison functions.
 */
SIMD_F128_INLINE int simd_f128_cmp(simd_f128 a, simd_f128 b);

// standard boolean comparisons
SIMD_F128_INLINE int simd_f128_eq(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE int simd_f128_gt(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE int simd_f128_lt(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE int simd_f128_ge(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE int simd_f128_le(simd_f128 a, simd_f128 b);

// utility math functions
SIMD_F128_INLINE int simd_f128_isnan(simd_f128 x);
SIMD_F128_INLINE int simd_f128_isinf(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_abs(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_min(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_max(simd_f128 a, simd_f128 b);

#ifdef __cplusplus
}
#endif

// ██ ███    ███ ██████  ██      ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██ 
// ██ ████  ████ ██   ██ ██      ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██ 
// ██ ██ ████ ██ ██████  ██      █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██ 
// ██ ██  ██  ██ ██      ██      ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██ 
// ██ ██      ██ ██      ███████ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████ 
//
// >>implementation logic

SIMD_F128_INLINE int simd_f128_cmp(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    /*
     * in double-double, we compare the hi part first.
     * if hi is different, that's our result.
     * if hi is the same, we have to look at the lo part (the error)
     * to see which one is actually bigger.
     *
     * note: if either operand is NaN, all comparisons return false,
     * so we fall through to return 0. callers that need IEEE 754
     * NaN semantics should check simd_f128_isnan() first, or use
     * the boolean comparison wrappers below which handle NaN.
     */
    if (ahi < bhi) return -1;
    if (ahi > bhi) return 1;
    if (alo < blo) return -1;
    if (alo > blo) return 1;
    return 0;
}

SIMD_F128_INLINE int simd_f128_eq(simd_f128 a, simd_f128 b) {
    /* IEEE 754: NaN != NaN */
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) == 0;
}

SIMD_F128_INLINE int simd_f128_gt(simd_f128 a, simd_f128 b) {
    /* IEEE 754: NaN is not greater than anything */
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) > 0;
}

SIMD_F128_INLINE int simd_f128_lt(simd_f128 a, simd_f128 b) {
    /* IEEE 754: NaN is not less than anything */
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) < 0;
}

SIMD_F128_INLINE int simd_f128_ge(simd_f128 a, simd_f128 b) {
    /* IEEE 754: NaN is not >= anything */
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) >= 0;
}

SIMD_F128_INLINE int simd_f128_le(simd_f128 a, simd_f128 b) {
    /* IEEE 754: NaN is not <= anything */
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) <= 0;
}

SIMD_F128_INLINE int simd_f128_isnan(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    return isnan(hi);
}

SIMD_F128_INLINE int simd_f128_isinf(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    return isinf(hi);
}

SIMD_F128_INLINE simd_f128 simd_f128_abs(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    
    /*
     * if the number is negative, we flip both hi and lo.
     * check hi first, but if hi is 0 (like -0.0), check lo.
     */
    if (hi < 0.0 || (hi == 0.0 && lo < 0.0)) {
        return simd_f128_neg(x);
    }
    return x;
}

SIMD_F128_INLINE simd_f128 simd_f128_min(simd_f128 a, simd_f128 b) {
    return (simd_f128_cmp(a, b) <= 0) ? a : b;
}

SIMD_F128_INLINE simd_f128 simd_f128_max(simd_f128 a, simd_f128 b) {
    return (simd_f128_cmp(a, b) >= 0) ? a : b;
}

#endif // simd_f128_utils_h
