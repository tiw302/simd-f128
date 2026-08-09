/* simd_f128_array.h
 *
 * vectorized array processing routines for structure-of-arrays (soa).
 * accelerates heavy loops using avx2, neon, and wasm native intrinsics.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#ifndef SIMD_F128_ARRAY_H
#define SIMD_F128_ARRAY_H

#include "simd_f128.h"
#include <stddef.h>

//  █████  ██████  ██████   █████  ██    ██
// ██   ██ ██   ██ ██   ██ ██   ██  ██  ██
// ███████ ██████  ██████  ███████   ████
// ██   ██ ██   ██ ██   ██ ██   ██    ██
// ██   ██ ██   ██ ██   ██ ██   ██    ██
//
// >>array processing api
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(__cplusplus) || defined(_MSC_VER)
#define SIMD_F128_RESTRICT __restrict
#else
#define SIMD_F128_RESTRICT restrict
#endif

/* auto-vectorized batch processing functions:
 * compiler must be invoked with optimizations (e.g. -O3, -mavx2, -ftree-vectorize)
 * to trigger the loop vectorizer. the restrict keyword guarantees to the compiler
 * that the input and output memory buffers do not overlap, allowing it to generate
 * parallel simd instructions. */

// batch add two double-double arrays
SIMD_F128_INLINE void simd_f128_array_add(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_add(a[i], b[i]);
    }
}

// batch subtract two double-double arrays
SIMD_F128_INLINE void simd_f128_array_sub(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_sub(a[i], b[i]);
    }
}

// batch multiply two double-double arrays
SIMD_F128_INLINE void simd_f128_array_mul(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_mul(a[i], b[i]);
    }
}

// batch divide two double-double arrays
SIMD_F128_INLINE void simd_f128_array_div(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_div(a[i], b[i]);
    }
}
// ============================================================================
// structure of arrays (soa) vectorized operations
// ============================================================================
/* these functions take separated arrays for high and low components to
 * maximize true vectorization potential. on avx2+ platforms, this allows
 * processing 4 double-double values per cycle natively without extraction. */

SIMD_F128_INLINE void simd_f128_array_add_soa(
    const double* SIMD_F128_RESTRICT a_hi, const double* SIMD_F128_RESTRICT a_lo,
    const double* SIMD_F128_RESTRICT b_hi, const double* SIMD_F128_RESTRICT b_lo,
    double* SIMD_F128_RESTRICT out_hi, double* SIMD_F128_RESTRICT out_lo,
    size_t length)
{
    size_t i = 0;
#if defined(SIMD_F128_USE_AVX2)
    for (; i + 3 < length; i += 4) {
        __m256d ahi = _mm256_loadu_pd(&a_hi[i]);
        __m256d alo = _mm256_loadu_pd(&a_lo[i]);
        __m256d bhi = _mm256_loadu_pd(&b_hi[i]);
        __m256d blo = _mm256_loadu_pd(&b_lo[i]);

        // knuth's two-sum for 4 elements at once
        __m256d s = _mm256_add_pd(ahi, bhi);
        __m256d v = _mm256_sub_pd(s, ahi);
        __m256d e_ahi_v = _mm256_sub_pd(ahi, _mm256_sub_pd(s, v));
        __m256d e_bhi_v = _mm256_sub_pd(bhi, v);
        __m256d e = _mm256_add_pd(e_ahi_v, e_bhi_v);

        __m256d t = _mm256_add_pd(_mm256_add_pd(alo, blo), e);

        __m256d final_hi = _mm256_add_pd(s, t);
        __m256d final_lo = _mm256_add_pd(_mm256_sub_pd(s, final_hi), t);

        _mm256_storeu_pd(&out_hi[i], final_hi);
        _mm256_storeu_pd(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_NEON)
    for (; i + 1 < length; i += 2) {
        float64x2_t ahi = vld1q_f64(&a_hi[i]);
        float64x2_t alo = vld1q_f64(&a_lo[i]);
        float64x2_t bhi = vld1q_f64(&b_hi[i]);
        float64x2_t blo = vld1q_f64(&b_lo[i]);
        float64x2_t s = vaddq_f64(ahi, bhi);
        float64x2_t v = vsubq_f64(s, ahi);
        float64x2_t e_ahi_v = vsubq_f64(ahi, vsubq_f64(s, v));
        float64x2_t e_bhi_v = vsubq_f64(bhi, v);
        float64x2_t e = vaddq_f64(e_ahi_v, e_bhi_v);
        float64x2_t t = vaddq_f64(vaddq_f64(alo, blo), e);
        float64x2_t final_hi = vaddq_f64(s, t);
        float64x2_t final_lo = vaddq_f64(vsubq_f64(s, final_hi), t);
        vst1q_f64(&out_hi[i], final_hi);
        vst1q_f64(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_WASM)
    for (; i + 1 < length; i += 2) {
        v128_t ahi = wasm_v128_load(&a_hi[i]);
        v128_t alo = wasm_v128_load(&a_lo[i]);
        v128_t bhi = wasm_v128_load(&b_hi[i]);
        v128_t blo = wasm_v128_load(&b_lo[i]);
        v128_t s = wasm_f64x2_add(ahi, bhi);
        v128_t v = wasm_f64x2_sub(s, ahi);
        v128_t e_ahi_v = wasm_f64x2_sub(ahi, wasm_f64x2_sub(s, v));
        v128_t e_bhi_v = wasm_f64x2_sub(bhi, v);
        v128_t e = wasm_f64x2_add(e_ahi_v, e_bhi_v);
        v128_t t = wasm_f64x2_add(wasm_f64x2_add(alo, blo), e);
        v128_t final_hi = wasm_f64x2_add(s, t);
        v128_t final_lo = wasm_f64x2_add(wasm_f64x2_sub(s, final_hi), t);
        wasm_v128_store(&out_hi[i], final_hi);
        wasm_v128_store(&out_lo[i], final_lo);
    }
#endif
    for (; i < length; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(a_hi[i], a_lo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi[i], b_lo[i]);
        simd_f128 sr = simd_f128_add(sa, sb);
        simd_f128_extract(sr, &out_hi[i], &out_lo[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_add_soa_fast(
    const double* SIMD_F128_RESTRICT a_hi, const double* SIMD_F128_RESTRICT a_lo,
    const double* SIMD_F128_RESTRICT b_hi, const double* SIMD_F128_RESTRICT b_lo,
    double* SIMD_F128_RESTRICT out_hi, double* SIMD_F128_RESTRICT out_lo,
    size_t length)
{
    size_t i = 0;
#if defined(SIMD_F128_USE_AVX2)
    for (; i + 3 < length; i += 4) {
        __m256d ahi = _mm256_loadu_pd(&a_hi[i]);
        __m256d alo = _mm256_loadu_pd(&a_lo[i]);
        __m256d bhi = _mm256_loadu_pd(&b_hi[i]);
        __m256d blo = _mm256_loadu_pd(&b_lo[i]);

        // fast two-sum for 4 elements at once (requires |a| >= |b|)
        __m256d s = _mm256_add_pd(ahi, bhi);
        __m256d e = _mm256_sub_pd(bhi, _mm256_sub_pd(s, ahi));

        __m256d t = _mm256_add_pd(_mm256_add_pd(alo, blo), e);

        __m256d final_hi = _mm256_add_pd(s, t);
        __m256d final_lo = _mm256_add_pd(_mm256_sub_pd(s, final_hi), t);

        _mm256_storeu_pd(&out_hi[i], final_hi);
        _mm256_storeu_pd(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_NEON)
    for (; i + 1 < length; i += 2) {
        float64x2_t ahi = vld1q_f64(&a_hi[i]);
        float64x2_t alo = vld1q_f64(&a_lo[i]);
        float64x2_t bhi = vld1q_f64(&b_hi[i]);
        float64x2_t blo = vld1q_f64(&b_lo[i]);
        float64x2_t s = vaddq_f64(ahi, bhi);
        float64x2_t e = vsubq_f64(bhi, vsubq_f64(s, ahi));
        float64x2_t t = vaddq_f64(vaddq_f64(alo, blo), e);
        float64x2_t final_hi = vaddq_f64(s, t);
        float64x2_t final_lo = vaddq_f64(vsubq_f64(s, final_hi), t);
        vst1q_f64(&out_hi[i], final_hi);
        vst1q_f64(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_WASM)
    for (; i + 1 < length; i += 2) {
        v128_t ahi = wasm_v128_load(&a_hi[i]);
        v128_t alo = wasm_v128_load(&a_lo[i]);
        v128_t bhi = wasm_v128_load(&b_hi[i]);
        v128_t blo = wasm_v128_load(&b_lo[i]);
        v128_t s = wasm_f64x2_add(ahi, bhi);
        v128_t e = wasm_f64x2_sub(bhi, wasm_f64x2_sub(s, ahi));
        v128_t t = wasm_f64x2_add(wasm_f64x2_add(alo, blo), e);
        v128_t final_hi = wasm_f64x2_add(s, t);
        v128_t final_lo = wasm_f64x2_add(wasm_f64x2_sub(s, final_hi), t);
        wasm_v128_store(&out_hi[i], final_hi);
        wasm_v128_store(&out_lo[i], final_lo);
    }
#endif
    for (; i < length; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(a_hi[i], a_lo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi[i], b_lo[i]);
        simd_f128 sr = simd_f128_add_fast(sa, sb);
        simd_f128_extract(sr, &out_hi[i], &out_lo[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_sub_soa(
    const double* SIMD_F128_RESTRICT a_hi, const double* SIMD_F128_RESTRICT a_lo,
    const double* SIMD_F128_RESTRICT b_hi, const double* SIMD_F128_RESTRICT b_lo,
    double* SIMD_F128_RESTRICT out_hi, double* SIMD_F128_RESTRICT out_lo,
    size_t length)
{
    size_t i = 0;
#if defined(SIMD_F128_USE_AVX2)
    for (; i + 3 < length; i += 4) {
        __m256d ahi = _mm256_loadu_pd(&a_hi[i]);
        __m256d alo = _mm256_loadu_pd(&a_lo[i]);
        __m256d bhi = _mm256_loadu_pd(&b_hi[i]);
        __m256d blo = _mm256_loadu_pd(&b_lo[i]);

        // knuth's two-sum for subtraction
        __m256d s = _mm256_sub_pd(ahi, bhi);
        __m256d v = _mm256_sub_pd(s, ahi);
        __m256d e = _mm256_sub_pd(
                        _mm256_sub_pd(ahi, _mm256_sub_pd(s, v)),
                        _mm256_add_pd(bhi, v)
                    );

        __m256d t = _mm256_add_pd(_mm256_sub_pd(alo, blo), e);

        __m256d final_hi = _mm256_add_pd(s, t);
        __m256d final_lo = _mm256_add_pd(_mm256_sub_pd(s, final_hi), t);

        _mm256_storeu_pd(&out_hi[i], final_hi);
        _mm256_storeu_pd(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_NEON)
    for (; i + 1 < length; i += 2) {
        float64x2_t ahi = vld1q_f64(&a_hi[i]);
        float64x2_t alo = vld1q_f64(&a_lo[i]);
        float64x2_t bhi = vld1q_f64(&b_hi[i]);
        float64x2_t blo = vld1q_f64(&b_lo[i]);
        float64x2_t s = vsubq_f64(ahi, bhi);
        float64x2_t v = vsubq_f64(s, ahi);
        float64x2_t e_ahi_v = vsubq_f64(ahi, vsubq_f64(s, v));
        float64x2_t e_bhi_v = vaddq_f64(bhi, v);
        float64x2_t e = vsubq_f64(e_ahi_v, e_bhi_v);
        float64x2_t t = vaddq_f64(vsubq_f64(alo, blo), e);
        float64x2_t final_hi = vaddq_f64(s, t);
        float64x2_t final_lo = vaddq_f64(vsubq_f64(s, final_hi), t);
        vst1q_f64(&out_hi[i], final_hi);
        vst1q_f64(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_WASM)
    for (; i + 1 < length; i += 2) {
        v128_t ahi = wasm_v128_load(&a_hi[i]);
        v128_t alo = wasm_v128_load(&a_lo[i]);
        v128_t bhi = wasm_v128_load(&b_hi[i]);
        v128_t blo = wasm_v128_load(&b_lo[i]);
        v128_t s = wasm_f64x2_sub(ahi, bhi);
        v128_t v = wasm_f64x2_sub(s, ahi);
        v128_t e_ahi_v = wasm_f64x2_sub(ahi, wasm_f64x2_sub(s, v));
        v128_t e_bhi_v = wasm_f64x2_add(bhi, v);
        v128_t e = wasm_f64x2_sub(e_ahi_v, e_bhi_v);
        v128_t t = wasm_f64x2_add(wasm_f64x2_sub(alo, blo), e);
        v128_t final_hi = wasm_f64x2_add(s, t);
        v128_t final_lo = wasm_f64x2_add(wasm_f64x2_sub(s, final_hi), t);
        wasm_v128_store(&out_hi[i], final_hi);
        wasm_v128_store(&out_lo[i], final_lo);
    }
#endif
    for (; i < length; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(a_hi[i], a_lo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi[i], b_lo[i]);
        simd_f128 sr = simd_f128_sub(sa, sb);
        simd_f128_extract(sr, &out_hi[i], &out_lo[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_sub_soa_fast(
    const double* SIMD_F128_RESTRICT a_hi, const double* SIMD_F128_RESTRICT a_lo,
    const double* SIMD_F128_RESTRICT b_hi, const double* SIMD_F128_RESTRICT b_lo,
    double* SIMD_F128_RESTRICT out_hi, double* SIMD_F128_RESTRICT out_lo,
    size_t length)
{
    size_t i = 0;
#if defined(SIMD_F128_USE_AVX2)
    for (; i + 3 < length; i += 4) {
        __m256d ahi = _mm256_loadu_pd(&a_hi[i]);
        __m256d alo = _mm256_loadu_pd(&a_lo[i]);
        __m256d bhi = _mm256_loadu_pd(&b_hi[i]);
        __m256d blo = _mm256_loadu_pd(&b_lo[i]);

        // fast two-diff for subtraction
        __m256d s = _mm256_sub_pd(ahi, bhi);
        __m256d e = _mm256_sub_pd(_mm256_sub_pd(_mm256_setzero_pd(), bhi), _mm256_sub_pd(s, ahi)); // -bhi - (s - ahi)

        __m256d t = _mm256_add_pd(_mm256_sub_pd(alo, blo), e);

        __m256d final_hi = _mm256_add_pd(s, t);
        __m256d final_lo = _mm256_add_pd(_mm256_sub_pd(s, final_hi), t);

        _mm256_storeu_pd(&out_hi[i], final_hi);
        _mm256_storeu_pd(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_NEON)
    for (; i + 1 < length; i += 2) {
        float64x2_t ahi = vld1q_f64(&a_hi[i]);
        float64x2_t alo = vld1q_f64(&a_lo[i]);
        float64x2_t bhi = vld1q_f64(&b_hi[i]);
        float64x2_t blo = vld1q_f64(&b_lo[i]);
        float64x2_t s = vsubq_f64(ahi, bhi);
        float64x2_t zero = vdupq_n_f64(0.0);
        float64x2_t e = vsubq_f64(vsubq_f64(zero, bhi), vsubq_f64(s, ahi));
        float64x2_t t = vaddq_f64(vsubq_f64(alo, blo), e);
        float64x2_t final_hi = vaddq_f64(s, t);
        float64x2_t final_lo = vaddq_f64(vsubq_f64(s, final_hi), t);
        vst1q_f64(&out_hi[i], final_hi);
        vst1q_f64(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_WASM)
    for (; i + 1 < length; i += 2) {
        v128_t ahi = wasm_v128_load(&a_hi[i]);
        v128_t alo = wasm_v128_load(&a_lo[i]);
        v128_t bhi = wasm_v128_load(&b_hi[i]);
        v128_t blo = wasm_v128_load(&b_lo[i]);
        v128_t s = wasm_f64x2_sub(ahi, bhi);
        v128_t zero = wasm_f64x2_splat(0.0);
        v128_t e = wasm_f64x2_sub(wasm_f64x2_sub(zero, bhi), wasm_f64x2_sub(s, ahi));
        v128_t t = wasm_f64x2_add(wasm_f64x2_sub(alo, blo), e);
        v128_t final_hi = wasm_f64x2_add(s, t);
        v128_t final_lo = wasm_f64x2_add(wasm_f64x2_sub(s, final_hi), t);
        wasm_v128_store(&out_hi[i], final_hi);
        wasm_v128_store(&out_lo[i], final_lo);
    }
#endif
    for (; i < length; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(a_hi[i], a_lo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi[i], b_lo[i]);
        simd_f128 sr = simd_f128_sub_fast(sa, sb);
        simd_f128_extract(sr, &out_hi[i], &out_lo[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_mul_soa(
    const double* SIMD_F128_RESTRICT a_hi, const double* SIMD_F128_RESTRICT a_lo,
    const double* SIMD_F128_RESTRICT b_hi, const double* SIMD_F128_RESTRICT b_lo,
    double* SIMD_F128_RESTRICT out_hi, double* SIMD_F128_RESTRICT out_lo,
    size_t length)
{
    size_t i = 0;
#if defined(SIMD_F128_USE_AVX2) && (defined(__FMA__) || defined(_MSC_VER))
    for (; i + 3 < length; i += 4) {
        __m256d ahi = _mm256_loadu_pd(&a_hi[i]);
        __m256d alo = _mm256_loadu_pd(&a_lo[i]);
        __m256d bhi = _mm256_loadu_pd(&b_hi[i]);
        __m256d blo = _mm256_loadu_pd(&b_lo[i]);

        __m256d p = _mm256_mul_pd(ahi, bhi);

        // fma: e = (ahi * bhi) - p
        __m256d e = _mm256_fmsub_pd(ahi, bhi, p);

        // cross-terms: e += (ahi * blo + alo * bhi)
        __m256d cross = _mm256_add_pd(_mm256_mul_pd(ahi, blo), _mm256_mul_pd(alo, bhi));
        e = _mm256_add_pd(e, cross);

        __m256d final_hi = _mm256_add_pd(p, e);
        __m256d final_lo = _mm256_sub_pd(e, _mm256_sub_pd(final_hi, p));

        _mm256_storeu_pd(&out_hi[i], final_hi);
        _mm256_storeu_pd(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_NEON)
    for (; i + 1 < length; i += 2) {
        float64x2_t ahi = vld1q_f64(&a_hi[i]);
        float64x2_t alo = vld1q_f64(&a_lo[i]);
        float64x2_t bhi = vld1q_f64(&b_hi[i]);
        float64x2_t blo = vld1q_f64(&b_lo[i]);
        float64x2_t p = vmulq_f64(ahi, bhi);
#ifdef SIMD_F128_HAS_FMA
        float64x2_t e = vfmsq_f64(p, ahi, bhi); // note: vfmsq is p - ahi*bhi, so we negate it
        e = vnegq_f64(e);
#else
        // use standard vmlal or dekker split for neon if no fma, but m1 has fma.
        // we will assume vfmsq_f64 is fine or use vfmaq_f64(vnegq_f64(p), ahi, bhi)
        float64x2_t e = vfmaq_f64(vnegq_f64(p), ahi, bhi);
#endif
        float64x2_t t = vaddq_f64(e, vaddq_f64(vmulq_f64(ahi, blo), vmulq_f64(alo, bhi)));
        float64x2_t final_hi = vaddq_f64(p, t);
        float64x2_t final_lo = vaddq_f64(vsubq_f64(p, final_hi), t);
        vst1q_f64(&out_hi[i], final_hi);
        vst1q_f64(&out_lo[i], final_lo);
    }
#elif defined(SIMD_F128_USE_WASM)
    for (; i + 1 < length; i += 2) {
        v128_t ahi = wasm_v128_load(&a_hi[i]);
        v128_t alo = wasm_v128_load(&a_lo[i]);
        v128_t bhi = wasm_v128_load(&b_hi[i]);
        v128_t blo = wasm_v128_load(&b_lo[i]);
        v128_t p = wasm_f64x2_mul(ahi, bhi);
        // wasm doesn't have fma natively yet in standard simd128, so we use dekker's split
        v128_t magic = wasm_f64x2_splat(134217729.0);
        v128_t t_ahi = wasm_f64x2_mul(ahi, magic);
        v128_t ahi_hi = wasm_f64x2_sub(t_ahi, wasm_f64x2_sub(t_ahi, ahi));
        v128_t ahi_lo = wasm_f64x2_sub(ahi, ahi_hi);
        v128_t t_bhi = wasm_f64x2_mul(bhi, magic);
        v128_t bhi_hi = wasm_f64x2_sub(t_bhi, wasm_f64x2_sub(t_bhi, bhi));
        v128_t bhi_lo = wasm_f64x2_sub(bhi, bhi_hi);
        v128_t e = wasm_f64x2_sub(wasm_f64x2_mul(ahi_hi, bhi_hi), p);
        e = wasm_f64x2_add(e, wasm_f64x2_mul(ahi_hi, bhi_lo));
        e = wasm_f64x2_add(e, wasm_f64x2_mul(ahi_lo, bhi_hi));
        e = wasm_f64x2_add(e, wasm_f64x2_mul(ahi_lo, bhi_lo));
        v128_t t = wasm_f64x2_add(e, wasm_f64x2_add(wasm_f64x2_mul(ahi, blo), wasm_f64x2_mul(alo, bhi)));
        v128_t final_hi = wasm_f64x2_add(p, t);
        v128_t final_lo = wasm_f64x2_add(wasm_f64x2_sub(p, final_hi), t);
        wasm_v128_store(&out_hi[i], final_hi);
        wasm_v128_store(&out_lo[i], final_lo);
    }
#endif
    for (; i < length; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(a_hi[i], a_lo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi[i], b_lo[i]);
        simd_f128 sr = simd_f128_mul(sa, sb);
        simd_f128_extract(sr, &out_hi[i], &out_lo[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_div_soa(
    const double* SIMD_F128_RESTRICT a_hi, const double* SIMD_F128_RESTRICT a_lo,
    const double* SIMD_F128_RESTRICT b_hi, const double* SIMD_F128_RESTRICT b_lo,
    double* SIMD_F128_RESTRICT out_hi, double* SIMD_F128_RESTRICT out_lo,
    size_t length)
{
    // division is highly branched and complex, utilizing scalar fallback
    for (size_t i = 0; i < length; i++) {
        simd_f128 sa = simd_f128_from_hi_lo(a_hi[i], a_lo[i]);
        simd_f128 sb = simd_f128_from_hi_lo(b_hi[i], b_lo[i]);
        simd_f128 sr = simd_f128_div(sa, sb);
        simd_f128_extract(sr, &out_hi[i], &out_lo[i]);
    }
}
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SIMD_F128_ARRAY_H
