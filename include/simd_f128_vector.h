// updated 2026-05-23

#ifndef SIMD_F128_VECTOR_H
#define SIMD_F128_VECTOR_H

#include "simd_f128.h"

// ██████  ███████ ██████  ███████ ███████  ██████ ████████ 
// ██   ██ ██      ██   ██ ██      ██      ██         ██    
// ██████  █████   ██████  █████   █████   ██         ██    
// ██      ██      ██   ██ ██      ██      ██         ██    
// ██      ███████ ██   ██ ███████ ███████  ██████    ██    
//
// >>vectorized api (simd_f128x4 for AVX2)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SIMD_F128_USE_AVX2)

/*
 * simd_f128x4 processes FOUR double-doubles simultaneously.
 * this is the pinnacle of parallel performance on x86_64.
 */
typedef struct {
    __m256d hi;
    __m256d lo;
} simd_f128x4;

SIMD_F128_INLINE simd_f128x4 simd_f128x4_from_doubles(double d0, double d1, double d2, double d3) {
    simd_f128x4 res;
    res.hi = _mm256_setr_pd(d0, d1, d2, d3);
    res.lo = _mm256_setzero_pd();
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_add(simd_f128x4 a, simd_f128x4 b) {
    __m256d s = _mm256_add_pd(a.hi, b.hi);
    __m256d v = _mm256_sub_pd(s, a.hi);
    __m256d e = _mm256_add_pd(_mm256_sub_pd(a.hi, _mm256_sub_pd(s, v)), _mm256_sub_pd(b.hi, v));
    __m256d t = _mm256_add_pd(_mm256_add_pd(a.lo, b.lo), e);
    
    simd_f128x4 res;
    res.hi = _mm256_add_pd(s, t);
    res.lo = _mm256_sub_pd(t, _mm256_sub_pd(res.hi, s));
    return res;
}

SIMD_F128_INLINE simd_f128x4 simd_f128x4_mul(simd_f128x4 a, simd_f128x4 b) {
    __m256d hi_prod = _mm256_mul_pd(a.hi, b.hi);
    
    // dekker's split or fma natively on avx2
#if defined(__FMA__)
    __m256d err = _mm256_fmsub_pd(a.hi, b.hi, hi_prod);
#else
    // simple split for avx2 without fma (rare, but happens)
    __m256d c = _mm256_set1_pd(134217729.0); // 2^27 + 1
    __m256d up = _mm256_mul_pd(a.hi, c);
    __m256d vp = _mm256_mul_pd(b.hi, c);
    __m256d u1 = _mm256_sub_pd(up, _mm256_sub_pd(up, a.hi));
    __m256d u2 = _mm256_sub_pd(a.hi, u1);
    __m256d v1 = _mm256_sub_pd(vp, _mm256_sub_pd(vp, b.hi));
    __m256d v2 = _mm256_sub_pd(b.hi, v1);
    __m256d err = _mm256_add_pd(_mm256_add_pd(_mm256_add_pd(_mm256_sub_pd(_mm256_mul_pd(u1, v1), hi_prod), _mm256_mul_pd(u1, v2)), _mm256_mul_pd(u2, v1)), _mm256_mul_pd(u2, v2));
#endif

    __m256d lo_prod = _mm256_add_pd(_mm256_add_pd(_mm256_mul_pd(a.hi, b.lo), _mm256_mul_pd(a.lo, b.hi)), err);
    
    simd_f128x4 res;
    res.hi = _mm256_add_pd(hi_prod, lo_prod);
    res.lo = _mm256_sub_pd(lo_prod, _mm256_sub_pd(res.hi, hi_prod));
    return res;
}

#endif // SIMD_F128_USE_AVX2

#ifdef __cplusplus
}
#endif

#endif // SIMD_F128_VECTOR_H
