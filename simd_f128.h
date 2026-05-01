#ifndef SIMD_F128_H
#define SIMD_F128_H

/*
    simd_f128.h -- high-performance 128-bit (double-double) arithmetic for SIMD.

    Project URL: https://github.com/tiw302/simd-f128

    Do this:
        #define SIMD_F128_IMPLEMENTATION
    before you include this file in *one* C or C++ file to create the
    implementation.

    technical background:
    ---------------------
    this library uses "double-double" arithmetic. basically, we represent a
    high-precision number as the sum of two 64-bit doubles (hi + lo).
    this gives us about 31 decimal digits of precision, which is roughly
    the same as quad precision (f128) but much faster because it uses
    hardware double-precision units.

    simd optimization:
    ------------------
    we've got backends for pretty much everything:
    - avx2:     x86_64 modern (haswell+, ryzen+)
    - sse2:     x86_64 older or low-power
    - neon:     arm64 (apple silicon, graviton, android)
    - wasm:     webassembly with simd128
    - scalar:   fallback for everything else (risc-v, ppc, etc.)

    license:
    --------
    mit license
    copyright (c) 2026 jirawat siripuk
*/

#include <stdint.h>
#include <math.h>

//  █████  ██████   ██████ ██   ██
// ██   ██ ██   ██ ██      ██   ██
// ███████ ██████  ██      ███████
// ██   ██ ██   ██ ██      ██   ██
// ██   ██ ██   ██  ██████ ██   ██
//
// >>arch detection

/* we check for avx2 first since it's the fastest on x86 */
#if defined(__AVX2__)
    #define SIMD_F128_USE_AVX2
    #include <immintrin.h>

/* then sse2 as a fallback for older x86 or when avx2 is disabled */
#elif defined(__SSE2__) || defined(_M_AMD64) || defined(_M_X64)
    #define SIMD_F128_USE_SSE2
    #include <emmintrin.h>

/* wasm simd128 is great for web apps that need the speed */
#elif defined(__wasm_simd128__)
    #define SIMD_F128_USE_WASM
    #include <wasm_simd128.h>

/* neon is standard on arm64 (aarch64) */
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_ARCH_ISA_A64)
    #define SIMD_F128_USE_NEON
    #include <arm_neon.h>

/* everything else goes to the scalar path */
#else
    #define SIMD_F128_USE_SCALAR
#endif

// ████████ ██    ██ ██████  ███████ ███████
//    ██     ██  ██  ██   ██ ██      ██
//    ██      ████   ██████  █████   ███████
//    ██       ██    ██      ██           ██
//    ██       ██    ██      ███████ ███████
//
// >>types

/*
 * simd_f128 - conceptually (hi + lo)
 *
 * we use 128-bit simd registers where possible to store both hi and lo
 * doubles in a single variable. this makes the code much cleaner and
 * helps the compiler optimize the data flow.
 */

#if defined(SIMD_F128_USE_AVX2) || defined(SIMD_F128_USE_SSE2)
    typedef __m128d simd_f128;
#elif defined(SIMD_F128_USE_WASM)
    typedef v128_t simd_f128;
#elif defined(SIMD_F128_USE_NEON)
    typedef float64x2_t simd_f128;
#else
    typedef struct {
        double hi;
        double lo;
    } simd_f128;
#endif

//  █████  ██████  ██
// ██   ██ ██   ██ ██
// ███████ ██████  ██
// ██   ██ ██      ██
// ██   ██ ██      ██
//
// >>api

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * we use always_inline to make sure there's no function call overhead.
 * for double-double arithmetic, the overhead of a function call can 
 * be significant compared to the actual math.
 */
#if defined(_MSC_VER)
    #define SIMD_F128_INLINE static __forceinline
#else
    #define SIMD_F128_INLINE static inline __attribute__((always_inline))
#endif

// initialization
SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d);

// arithmetic
SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x);

// extraction
SIMD_F128_INLINE void simd_f128_extract(simd_f128 x, double* hi, double* lo) {
#if defined(SIMD_F128_USE_AVX2) || defined(SIMD_F128_USE_SSE2)
    *hi = _mm_cvtsd_f64(x);
    *lo = _mm_cvtsd_f64(_mm_unpackhi_pd(x, x));
#elif defined(SIMD_F128_USE_WASM)
    *hi = wasm_f64x2_extract_lane(x, 0);
    *lo = wasm_f64x2_extract_lane(x, 1);
#elif defined(SIMD_F128_USE_NEON)
    *hi = vgetq_lane_f64(x, 0);
    *lo = vgetq_lane_f64(x, 1);
#else
    *hi = x.hi;
    *lo = x.lo;
#endif
}

#ifdef __cplusplus
}
#endif

// ██ ███    ███ ██████  ██
// ██ ████  ████ ██   ██ ██
// ██ ██ ████ ██ ██████  ██
// ██ ██  ██  ██ ██      ██
// ██ ██      ██ ██      ███████
//
// >>implementation

#ifdef SIMD_F128_IMPLEMENTATION

// ███    ███  █████  ████████ ██   ██
// ████  ████ ██   ██    ██    ██   ██
// ██ ████ ██ ███████    ██    ███████
// ██  ██  ██ ██   ██    ██    ██   ██
// ██      ██ ██   ██    ██    ██   ██
//
// >>math helpers

#if defined(SIMD_F128_USE_AVX2)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        /* hi = d, lo = 0.0 */
        return _mm_set_pd(0.0, d);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        /* 
         * knuth's two-sum algorithm. 
         * we basically do (a + b) and then calculate the exact error.
         */
        __m128d ahi = _mm_unpacklo_pd(a, a);
        __m128d bhi = _mm_unpacklo_pd(b, b);
        
        __m128d s = _mm_add_sd(ahi, bhi);
        __m128d v = _mm_sub_sd(s, ahi);
        __m128d e = _mm_add_sd(_mm_sub_sd(ahi, _mm_sub_sd(s, v)), _mm_sub_sd(bhi, v));

        /* mix in the low parts of the inputs */
        __m128d alo = _mm_unpackhi_pd(a, a);
        __m128d blo = _mm_unpackhi_pd(b, b);
        __m128d t = _mm_add_sd(_mm_add_sd(alo, blo), e);

        /* final normalization so that |hi| is as large as possible */
        __m128d final_hi = _mm_add_sd(s, t);
        __m128d final_lo = _mm_sub_sd(t, _mm_sub_sd(final_hi, s));

        return _mm_unpacklo_pd(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        /* simple subtraction by flipping the sign of b and adding */
        return simd_f128_add(a, _mm_xor_pd(b, _mm_set1_pd(-0.0)));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        /*
         * dekker's multiplication using hardware fma.
         * fma is super important here because it calculates (a*b + c)
         * with a single rounding step, so fma(a, b, - (a*b)) gives us
         * the exact error of the multiplication.
         */
        __m128d ahi = _mm_unpacklo_pd(a, a);
        __m128d bhi = _mm_unpacklo_pd(b, b);
        
        __m128d p = _mm_mul_sd(ahi, bhi);
        __m128d e = _mm_fmsub_sd(ahi, bhi, p);
        
        /* add cross-terms from the low parts */
        __m128d alo = _mm_unpackhi_pd(a, a);
        __m128d blo = _mm_unpackhi_pd(b, b);
        __m128d c1 = _mm_mul_sd(ahi, blo);
        __m128d c2 = _mm_mul_sd(alo, bhi);
        e = _mm_add_sd(e, _mm_add_sd(c1, c2));

        /* re-normalize */
        __m128d final_hi = _mm_add_sd(p, e);
        __m128d final_lo = _mm_sub_sd(e, _mm_sub_sd(final_hi, p));

        return _mm_unpacklo_pd(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        /* 
         * division is tricky. we use a few iterations of newton-raphson 
         * to get high precision for the reciprocal.
         */
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double r = 1.0 / bhi;
        r = r * (2.0 - bhi * r);
        r = r * (2.0 - bhi * r);

        double q = ahi * r;
        double qlo = fma(ahi, r, -q) + alo * r;

        double p = bhi * q;
        double plo = fma(bhi, q, -p) + blo * q + qlo * bhi;

        double final_hi = q + (ahi - p - plo) * r;
        double final_lo = (ahi - p - plo) * r - (final_hi - q);

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        /* same logic as division, using newton-raphson for the root */
        double xhi, xlo;
        simd_f128_extract(x, &xhi, &xlo);

        double y = 1.0 / sqrt(xhi);
        y = 0.5 * y * (3.0 - xhi * y * y);
        y = 0.5 * y * (3.0 - xhi * y * y);

        double z = xhi * y;
        double zlo = fma(xhi, y, -z) + xlo * y;

        double est = z * z;
        double estlo = fma(z, z, -est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        return _mm_set_pd(final_lo, final_hi);
    }

#elif defined(SIMD_F128_USE_SSE2)

    /* 
     * sse2 path is similar to avx2 but without fma intrinsics.
     * we have to use the standard fma() from math.h which might be slower
     * if the cpu doesn't have the instruction, but it's still accurate.
     */

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        return _mm_set_pd(0.0, d);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double s = ahi + bhi;
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);
        
        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        return simd_f128_add(a, _mm_xor_pd(b, _mm_set1_pd(-0.0)));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double p = ahi * bhi;
        double e = fma(ahi, bhi, -p);
        e += fma(ahi, blo, alo * bhi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);
        
        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double r = 1.0 / bhi;
        r = r * (2.0 - bhi * r);
        r = r * (2.0 - bhi * r);

        double q = ahi * r;
        double qlo = fma(ahi, r, -q) + alo * r;

        double p = bhi * q;
        double plo = fma(bhi, q, -p) + blo * q + qlo * bhi;

        double final_hi = q + (ahi - p - plo) * r;
        double final_lo = (ahi - p - plo) * r - (final_hi - q);

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi, xlo;
        simd_f128_extract(x, &xhi, &xlo);

        double y = 1.0 / sqrt(xhi);
        y = 0.5 * y * (3.0 - xhi * y * y);
        y = 0.5 * y * (3.0 - xhi * y * y);

        double z = xhi * y;
        double zlo = fma(xhi, y, -z) + xlo * y;

        double est = z * z;
        double estlo = fma(z, z, -est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        return _mm_set_pd(final_lo, final_hi);
    }

#elif defined(SIMD_F128_USE_WASM)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        return wasm_f64x2_make(d, 0.0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double s = ahi + bhi;
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);
        
        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        v128_t neg_mask = wasm_i64x2_const(0x8000000000000000ULL, 0x8000000000000000ULL);
        return simd_f128_add(a, wasm_v128_xor(b, neg_mask));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double p = ahi * bhi;
        double e = fma(ahi, bhi, -p);
        e += fma(ahi, blo, alo * bhi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);
        
        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double r = 1.0 / bhi;
        r = r * (2.0 - bhi * r);
        r = r * (2.0 - bhi * r);

        double q = ahi * r;
        double qlo = fma(ahi, r, -q) + alo * r;

        double p = bhi * q;
        double plo = fma(bhi, q, -p) + blo * q + qlo * bhi;

        double final_hi = q + (ahi - p - plo) * r;
        double final_lo = (ahi - p - plo) * r - (final_hi - q);

        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi, xlo;
        simd_f128_extract(x, &xhi, &xlo);

        double y = 1.0 / sqrt(xhi);
        y = 0.5 * y * (3.0 - xhi * y * y);
        y = 0.5 * y * (3.0 - xhi * y * y);

        double z = xhi * y;
        double zlo = fma(xhi, y, -z) + xlo * y;

        double est = z * z;
        double estlo = fma(z, z, -est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        return wasm_f64x2_make(final_hi, final_lo);
    }

#elif defined(SIMD_F128_USE_NEON)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        /* hi in index 0, lo in index 1 for neon */
        float64x2_t r = vdupq_n_f64(0.0);
        return vsetq_lane_f64(d, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 0);
        double alo = vgetq_lane_f64(a, 1);
        double bhi = vgetq_lane_f64(b, 0);
        double blo = vgetq_lane_f64(b, 1);

        double s = ahi + bhi;
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);
        
        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        float64x2_t r = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        return simd_f128_add(a, vnegq_f64(b));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 0);
        double alo = vgetq_lane_f64(a, 1);
        double bhi = vgetq_lane_f64(b, 0);
        double blo = vgetq_lane_f64(b, 1);

        double p = ahi * bhi;
        double e = fma(ahi, bhi, -p);
        e += fma(ahi, blo, alo * bhi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);

        float64x2_t r = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 0);
        double alo = vgetq_lane_f64(a, 1);
        double bhi = vgetq_lane_f64(b, 0);
        double blo = vgetq_lane_f64(b, 1);

        double r = 1.0 / bhi;
        r = r * (2.0 - bhi * r);
        r = r * (2.0 - bhi * r);

        double q = ahi * r;
        double qlo = fma(ahi, r, -q) + alo * r;

        double p = bhi * q;
        double plo = fma(bhi, q, -p) + blo * q + qlo * bhi;

        double final_hi = q + (ahi - p - plo) * r;
        double final_lo = (ahi - p - plo) * r - (final_hi - q);

        float64x2_t r_res = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r_res, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi = vgetq_lane_f64(x, 0);
        double xlo = vgetq_lane_f64(x, 1);

        double y = 1.0 / sqrt(xhi);
        y = 0.5 * y * (3.0 - xhi * y * y);
        y = 0.5 * y * (3.0 - xhi * y * y);

        double z = xhi * y;
        double zlo = fma(xhi, y, -z) + xlo * y;

        double est = z * z;
        double estlo = fma(z, z, -est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        float64x2_t r_res = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r_res, 0);
    }

#else
    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        simd_f128 res = {d, 0.0};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double s = a.hi + b.hi;
        double v = s - a.hi;
        double e = (a.hi - (s - v)) + (b.hi - v);
        
        double t = a.lo + b.lo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);
        
        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        b.hi = -b.hi; b.lo = -b.lo;
        return simd_f128_add(a, b);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double p = a.hi * b.hi;
        double e = fma(a.hi, b.hi, -p);
        e += fma(a.hi, b.lo, a.lo * b.hi);
        
        double final_hi = p + e;
        double final_lo = e - (final_hi - p);
        
        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double r = 1.0 / b.hi;
        r = r * (2.0 - b.hi * r);
        r = r * (2.0 - b.hi * r);

        double q = a.hi * r;
        double qlo = fma(a.hi, r, -q) + a.lo * r;

        double p = b.hi * q;
        double plo = fma(b.hi, q, -p) + b.lo * q + qlo * b.hi;

        double final_hi = q + (a.hi - p - plo) * r;
        double final_lo = (a.hi - p - plo) * r - (final_hi - q);

        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double y = 1.0 / sqrt(x.hi);
        y = 0.5 * y * (3.0 - x.hi * y * y);
        y = 0.5 * y * (3.0 - x.hi * y * y);

        double z = x.hi * y;
        double zlo = fma(x.hi, y, -z) + x.lo * y;

        double est = z * z;
        double estlo = fma(z, z, -est) + 2.0 * z * zlo;
        double err = (x.hi - est) - estlo + x.lo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        simd_f128 res = {final_hi, final_lo};
        return res;
    }
#endif

#endif // SIMD_F128_IMPLEMENTATION
#endif // SIMD_F128_H
