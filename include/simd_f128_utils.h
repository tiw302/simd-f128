// updated 2026-06-12
// spdx-license-identifier: mit
// copyright (c) 2026 jirawat siripuk

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

// compare two double-double numbers, returning -1 if a < b, 1 if a > b, and 0 if a == b
SIMD_F128_INLINE int simd_f128_cmp(simd_f128 a, simd_f128 b);

// check if two numbers are equal
SIMD_F128_INLINE int simd_f128_eq(simd_f128 a, simd_f128 b);

// check if a is strictly greater than b
SIMD_F128_INLINE int simd_f128_gt(simd_f128 a, simd_f128 b);

// check if a is strictly less than b
SIMD_F128_INLINE int simd_f128_lt(simd_f128 a, simd_f128 b);

// check if a is greater than or equal to b
SIMD_F128_INLINE int simd_f128_ge(simd_f128 a, simd_f128 b);

// check if a is less than or equal to b
SIMD_F128_INLINE int simd_f128_le(simd_f128 a, simd_f128 b);

// check if a number is nan
SIMD_F128_INLINE int simd_f128_isnan(simd_f128 x);

// check if a number is positive or negative infinity
SIMD_F128_INLINE int simd_f128_isinf(simd_f128 x);

// calculate the absolute value
SIMD_F128_INLINE simd_f128 simd_f128_abs(simd_f128 x);

// return the minimum of two numbers
SIMD_F128_INLINE simd_f128 simd_f128_min(simd_f128 a, simd_f128 b);

// return the maximum of two numbers
SIMD_F128_INLINE simd_f128 simd_f128_max(simd_f128 a, simd_f128 b);

#ifdef __cplusplus
}
#endif

// ██ ███    ███ ██████  ██      ███████ ███    ███ ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██ 
// ██ ████  ████ ██   ██ ██      ██      ████  ████ ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██ 
// ██ ██ ████ ██ ██████  ██      █████   ██ ████ ██ █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██ 
// ██ ██  ██  ██ ██      ██      ██      ██  ██  ██ ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██ 
// ██ ██      ██ ██      ███████ ███████ ██      ██ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████ 
//
// >>implementation logic

SIMD_F128_INLINE int simd_f128_cmp(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    // check for nan explicitly, since standard double comparison with nan is false.
    // we return a distinct value (-2) to signal unordered if someone relies on cmp directly.
    if (isnan(ahi) || isnan(bhi) || isnan(alo) || isnan(blo)) return -2;

    // double-double comparison logic:
    // compare the hi components first. if they are different, we can immediately return.
    // if the hi components are equal, we compare the lo components (residual error).
    if (ahi < bhi) return -1;
    if (ahi > bhi) return 1;
    if (alo < blo) return -1;
    if (alo > blo) return 1;
    return 0;
}

SIMD_F128_INLINE int simd_f128_eq(simd_f128 a, simd_f128 b) {
    // ieee-754: any comparison involving nan must return false (e.g. nan != nan)
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) == 0;
}

SIMD_F128_INLINE int simd_f128_gt(simd_f128 a, simd_f128 b) {
    // ieee-754: nan is not greater than anything
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) > 0;
}

SIMD_F128_INLINE int simd_f128_lt(simd_f128 a, simd_f128 b) {
    // ieee-754: nan is not less than anything
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) < 0;
}

SIMD_F128_INLINE int simd_f128_ge(simd_f128 a, simd_f128 b) {
    // ieee-754: nan is not >= anything
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) >= 0;
}

SIMD_F128_INLINE int simd_f128_le(simd_f128 a, simd_f128 b) {
    // ieee-754: nan is not <= anything
    if (simd_f128_isnan(a) || simd_f128_isnan(b)) return 0;
    return simd_f128_cmp(a, b) <= 0;
}

SIMD_F128_INLINE int simd_f128_isnan(simd_f128 x) {
    // check only the hi component since nan is defined at that level.
    // a well-formed double-double should never have lo == nan while hi is normal,
    // so checking only hi is correct and preserves the fast path.
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    return isnan(hi);
}

SIMD_F128_INLINE int simd_f128_isinf(simd_f128 x) {
    // check only the hi component for infinite values
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    return isinf(hi);
}

SIMD_F128_INLINE simd_f128 simd_f128_abs(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    
    // absolute value logic:
    // negate both components if hi is negative.
    // if hi is positive, keep signs.
    // if hi is zero (could be -0.0), force positive zero and absolute value of lo.
    if (hi > 0.0) return x;
    if (hi < 0.0) return simd_f128_neg(x);
    return simd_f128_from_hi_lo(0.0, fabs(lo));
}

SIMD_F128_INLINE simd_f128 simd_f128_min(simd_f128 a, simd_f128 b) {
    // return the smaller of the two double-double numbers, conforming to fmin nan rules
    int a_nan = simd_f128_isnan(a);
    int b_nan = simd_f128_isnan(b);
    if (a_nan && b_nan) return a;
    if (a_nan) return b;
    if (b_nan) return a;
    return (simd_f128_cmp(a, b) <= 0) ? a : b;
}

SIMD_F128_INLINE simd_f128 simd_f128_max(simd_f128 a, simd_f128 b) {
    // return the larger of the two double-double numbers, conforming to fmax nan rules
    int a_nan = simd_f128_isnan(a);
    int b_nan = simd_f128_isnan(b);
    if (a_nan && b_nan) return a;
    if (a_nan) return b;
    if (b_nan) return a;
    return (simd_f128_cmp(a, b) >= 0) ? a : b;
}

#endif // simd_f128_utils_h
