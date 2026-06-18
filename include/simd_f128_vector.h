// updated 2026-06-12
// spdx-license-identifier: mit
// copyright (c) 2026 jirawat siripuk

#ifndef SIMD_F128_VECTOR_H
#define SIMD_F128_VECTOR_H

#include "simd_f128.h"
#include "simd_f128_utils.h"

// ██████  ███████ ██████  ███████ ███████  ██████ ████████ 
// ██   ██ ██      ██   ██ ██      ██      ██         ██    
// ██████  █████   ██████  █████   █████   ██         ██    
// ██      ██      ██   ██ ██      ██      ██         ██    
// ██      ███████ ██   ██ ███████ ███████  ██████    ██    
//
// >>vectorized api (simd_f128x4 for avx2)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SIMD_F128_USE_AVX2)

/*
 * simd_f128x4 processes four double-doubles simultaneously.
 * this is the pinnacle of parallel performance on x86_64.
 *
 * warning: pure vectorized operations here may propagate nan and inf
 * differently than the scalar or standard inline functions, since
 * branches are omitted in favor of blend masks for performance.
 */
typedef struct {
    __m256d hi;
    __m256d lo;
} simd_f128x4;

// initialize a vectorized simd_f128x4 struct from four individual doubles
// hi registers are loaded with the values, lo registers are initialized to zero
SIMD_F128_INLINE simd_f128x4 simd_f128x4_from_doubles(double d0, double d1, double d2, double d3) {
    simd_f128x4 res;
    res.hi = _mm256_setr_pd(d0, d1, d2, d3);
    res.lo = _mm256_setzero_pd();
    return res;
}

// perform parallel addition of four double-double values using knuth's two-sum
SIMD_F128_INLINE simd_f128x4 simd_f128x4_add(simd_f128x4 a, simd_f128x4 b) {
    __m256d s = _mm256_add_pd(a.hi, b.hi);
    __m256d diff = _mm256_sub_pd(s, s);
    __m256d inf_mask = _mm256_cmp_pd(diff, diff, _CMP_UNORD_Q);
    __m256d v = _mm256_sub_pd(s, a.hi);
    __m256d e = _mm256_add_pd(_mm256_sub_pd(a.hi, _mm256_sub_pd(s, v)), _mm256_sub_pd(b.hi, v));
    __m256d t = _mm256_add_pd(_mm256_add_pd(a.lo, b.lo), e);
    
    simd_f128x4 res;
    res.hi = _mm256_add_pd(s, t);
    res.lo = _mm256_sub_pd(t, _mm256_sub_pd(res.hi, s));
    // if s is infinite or nan, set lo component to zero to prevent invalid values
    res.lo = _mm256_blendv_pd(res.lo, _mm256_setzero_pd(), inf_mask);
    return res;
}

// perform parallel multiplication of four double-double values
SIMD_F128_INLINE simd_f128x4 simd_f128x4_mul(simd_f128x4 a, simd_f128x4 b) {
    __m256d hi_prod = _mm256_mul_pd(a.hi, b.hi);
    
    // exact multiplication error estimation:
    // uses hardware fma if supported, otherwise falls back to a vector split method
#if defined(__FMA__)
    __m256d err = _mm256_fmsub_pd(a.hi, b.hi, hi_prod);
#else
    // vector split using dekker's constant (2^27 + 1)
    __m256d c = _mm256_set1_pd(134217729.0);
    __m256d up = _mm256_mul_pd(a.hi, c);
    __m256d vp = _mm256_mul_pd(b.hi, c);
    __m256d u1 = _mm256_sub_pd(up, _mm256_sub_pd(up, a.hi));
    __m256d u2 = _mm256_sub_pd(a.hi, u1);
    __m256d v1 = _mm256_sub_pd(vp, _mm256_sub_pd(vp, b.hi));
    __m256d v2 = _mm256_sub_pd(b.hi, v1);
    __m256d err = _mm256_add_pd(_mm256_add_pd(_mm256_add_pd(_mm256_sub_pd(_mm256_mul_pd(u1, v1), hi_prod), _mm256_mul_pd(u1, v2)), _mm256_mul_pd(u2, v1)), _mm256_mul_pd(u2, v2));
#endif

    // accumulate cross-terms and the exact multiplication roundoff error
    __m256d lo_prod = _mm256_add_pd(_mm256_add_pd(_mm256_mul_pd(a.hi, b.lo), _mm256_mul_pd(a.lo, b.hi)), err);
    
    simd_f128x4 res;
    res.hi = _mm256_add_pd(hi_prod, lo_prod);
    res.lo = _mm256_sub_pd(lo_prod, _mm256_sub_pd(res.hi, hi_prod));
    return res;
}

// negate all four double-double elements in parallel
SIMD_F128_INLINE simd_f128x4 simd_f128x4_neg(simd_f128x4 a) {
    simd_f128x4 res;
    res.hi = _mm256_xor_pd(a.hi, _mm256_set1_pd(-0.0));
    res.lo = _mm256_xor_pd(a.lo, _mm256_set1_pd(-0.0));
    return res;
}

// subtract four double-double elements in parallel
SIMD_F128_INLINE simd_f128x4 simd_f128x4_sub(simd_f128x4 a, simd_f128x4 b) {
    return simd_f128x4_add(a, simd_f128x4_neg(b));
}

// multiply four double-double elements by two in parallel
SIMD_F128_INLINE simd_f128x4 simd_f128x4_mul2(simd_f128x4 a) {
    simd_f128x4 res;
    res.hi = _mm256_add_pd(a.hi, a.hi);
    res.lo = _mm256_add_pd(a.lo, a.lo);
    return res;
}

// square all four double-double elements in parallel (a * a)
SIMD_F128_INLINE simd_f128x4 simd_f128x4_sqr(simd_f128x4 a) {
    return simd_f128x4_mul(a, a);
}

// compute absolute values of all four double-double elements in parallel
SIMD_F128_INLINE simd_f128x4 simd_f128x4_abs(simd_f128x4 a) {
    __m256d zero = _mm256_setzero_pd();
    __m256d sign_hi = _mm256_and_pd(a.hi, _mm256_set1_pd(-0.0));
    __m256d eq_zero = _mm256_cmp_pd(a.hi, zero, _CMP_EQ_OQ);
    __m256d abs_lo = _mm256_andnot_pd(_mm256_set1_pd(-0.0), a.lo);
    
    __m256d normal_hi = _mm256_xor_pd(a.hi, sign_hi);
    __m256d normal_lo = _mm256_xor_pd(a.lo, sign_hi);
    
    simd_f128x4 res;
    res.hi = _mm256_blendv_pd(normal_hi, zero, eq_zero);
    res.lo = _mm256_blendv_pd(normal_lo, abs_lo, eq_zero);
    return res;
}

// perform parallel division of four double-double elements lane-by-lane
SIMD_F128_INLINE simd_f128x4 simd_f128x4_div(simd_f128x4 a, simd_f128x4 b) {
    double ahi[4], alo[4], bhi[4], blo[4];
    _mm256_storeu_pd(ahi, a.hi);
    _mm256_storeu_pd(alo, a.lo);
    _mm256_storeu_pd(bhi, b.hi);
    _mm256_storeu_pd(blo, b.lo);
    
    double rhi[4], rlo[4];
    for (int i = 0; i < 4; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(ahi[i], alo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(bhi[i], blo[i]);
        simd_f128 sr = simd_f128_div(sa, sb);
        simd_f128_extract(sr, &rhi[i], &rlo[i]);
    }
    
    simd_f128x4 res;
    res.hi = _mm256_loadu_pd(rhi);
    res.lo = _mm256_loadu_pd(rlo);
    return res;
}

// perform parallel square root of four double-double elements lane-by-lane
SIMD_F128_INLINE simd_f128x4 simd_f128x4_sqrt(simd_f128x4 a) {
    double ahi[4], alo[4];
    _mm256_storeu_pd(ahi, a.hi);
    _mm256_storeu_pd(alo, a.lo);
    
    double rhi[4], rlo[4];
    for (int i = 0; i < 4; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(ahi[i], alo[i]);
        simd_f128 sr = simd_f128_sqrt(sa);
        simd_f128_extract(sr, &rhi[i], &rlo[i]);
    }
    
    simd_f128x4 res;
    res.hi = _mm256_loadu_pd(rhi);
    res.lo = _mm256_loadu_pd(rlo);
    return res;
}

#else // !defined(SIMD_F128_USE_AVX2)

/*
 * fallback implementation of simd_f128x4 for non-AVX2 platforms
 */
typedef struct {
    simd_f128 val[4];
} simd_f128x4;

SIMD_F128_INLINE simd_f128x4 simd_f128x4_from_doubles(double d0, double d1, double d2, double d3) {
    simd_f128x4 res;
    res.val[0] = simd_f128_from_double(d0);
    res.val[1] = simd_f128_from_double(d1);
    res.val[2] = simd_f128_from_double(d2);
    res.val[3] = simd_f128_from_double(d3);
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_add(simd_f128x4 a, simd_f128x4 b) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_add(a.val[i], b.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_mul(simd_f128x4 a, simd_f128x4 b) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_mul(a.val[i], b.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_neg(simd_f128x4 a) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_neg(a.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_sub(simd_f128x4 a, simd_f128x4 b) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_sub(a.val[i], b.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_mul2(simd_f128x4 a) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_add(a.val[i], a.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_sqr(simd_f128x4 a) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_mul(a.val[i], a.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_abs(simd_f128x4 a) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_abs(a.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_div(simd_f128x4 a, simd_f128x4 b) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_div(a.val[i], b.val[i]);
    }
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_sqrt(simd_f128x4 a) {
    simd_f128x4 res;
    for (int i = 0; i < 4; i++) {
        res.val[i] = simd_f128_sqrt(a.val[i]);
    }
    return res;
}

#endif // simd_f128_use_avx2

#ifdef __cplusplus
}
#endif

#endif // simd_f128_vector_h
