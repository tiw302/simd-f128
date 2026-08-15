/* simd_f128.h
 *
 * core definitions and primary arithmetic operations for 128-bit (double-double).
 * provides the foundational __m128d / double[2] hi/lo storage and math primitives.
 *
 * project url: https://github.com/tiw302/simd-f128
 *
 * usage:
 * #define SIMD_F128_IMPLEMENTATION
 * before you include this file in *one* c or c++ file to create the implementation.
 *
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 *
 * simd optimization:
 * ------------------
 * we've got backends for pretty much everything:
 * - avx2:     x86_64 modern (haswell+, ryzen+)
 * - sse2:     x86_64 older or low-power
 * - neon:     arm64 (apple silicon, graviton, android)
 * - wasm:     webassembly with simd128
 * - scalar:   fallback for everything else (risc-v, ppc, etc.)
 *
 * updated 2026-08-13
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

/*     /\_/\
 *    ( -.- )  <-- "precision: 31 digits. developer sanity: 0 digits."
 *   ==  Y  ==
 *    (  .  )
 *    (  .  ) */

#ifndef SIMD_F128_H
#define SIMD_F128_H

#include <math.h>
#include <stdint.h>

//  █████  ██████   ██████ ██   ██
// ██   ██ ██   ██ ██      ██   ██
// ███████ ██████  ██      ███████
// ██   ██ ██   ██ ██      ██   ██
// ██   ██ ██   ██  ██████ ██   ██
//
// >>arch detection
// we check for avx2 first since it's the fastest on x86
#if defined(__AVX2__)
#define SIMD_F128_USE_AVX2
#include <immintrin.h>

// wasm simd128 is great for web apps that need the speed
#elif defined(__wasm_simd128__)
#define SIMD_F128_USE_WASM
#include <wasm_simd128.h>

// then sse2 as a fallback for older x86 or when avx2 is disabled
#elif defined(__SSE2__) || defined(_M_AMD64) || defined(_M_X64)
#define SIMD_F128_USE_SSE2
#include <emmintrin.h>

// neon is standard on arm64 (aarch64)
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_ARCH_ISA_A64)
#define SIMD_F128_USE_NEON
#include <arm_neon.h>

// everything else goes to the scalar path
#else
#define SIMD_F128_USE_SCALAR
#endif
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__builtin_expect)
#define __builtin_expect(x, y) (x)
#endif
//    ██     ██  ██  ██   ██ ██      ██
//    ██      ████   ██████  █████   ███████
//    ██       ██    ██      ██           ██
//    ██       ██    ██      ███████ ███████
//
// >>types
/* conceptually (hi + lo) */
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
// ██   ██ ██   ██ ██
// ███████ ██████  ██
// ██   ██ ██      ██
// ██   ██ ██      ██
//
// >>api
#ifdef __cplusplus
extern "C" {
#endif

// gpu (cuda/hip) support
#if defined(__CUDACC__) || defined(__HIPCC__)
#define SIMD_F128_DEVICE __device__ __host__
#else
#define SIMD_F128_DEVICE
#endif

/* always_inline avoids function call overhead. */
#if defined(__CUDACC__) || defined(__HIPCC__)
#define SIMD_F128_INLINE SIMD_F128_DEVICE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define SIMD_F128_INLINE SIMD_F128_DEVICE static __forceinline
#else
#define SIMD_F128_INLINE SIMD_F128_DEVICE static inline __attribute__((always_inline))
#endif

/* initialization routines:
 * from_double: create 128-bit number from 64-bit double.
 * from_hi_lo: construct double-double from hi and lo. */
SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d);
SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo);

/* core arithmetic operations:
 * arithmetic on double-doubles is complex. addition and subtraction utilize
 * knuth's two-sum algorithm to meticulously track and preserve rounding errors.
 * multiplication uses hardware fma (fused multiply-accumulate) where available,
 * or falls back to dekker's split. division relies on a custom newton-raphson
 * or direct division iteration depending on the architecture. */
SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_rsqrt(simd_f128 x);

#ifdef __cplusplus
}
#endif

// extraction
// moved here to avoid odr issues in the c++ api section
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

// dekker's split method to estimate the roundoff error of a double product.
// used as a fallback when hardware fma instruction (fp_fast_fma) is not present.
SIMD_F128_INLINE double simd_f128_exact_mul_err(double a, double b, double p) {
#ifdef FP_FAST_FMA
    // use hardware fma if compiler flags detect fast hardware capability
    return fma(a, b, -p);
#else
    if (__builtin_expect(fabs(a) > 6.7e299 && fabs(b) < 1.0, 0)) {
        a *= 3.7252902984619140625e-09;  // 2^-28
        b *= 268435456.0;                // 2^28
    } else if (__builtin_expect(fabs(b) > 6.7e299 && fabs(a) < 1.0, 0)) {
        b *= 3.7252902984619140625e-09;
        a *= 268435456.0;
    }

    // split double value into high and low half-words
    double c, ahi, alo, bhi, blo;

    c = 134217729.0 * a;
    ahi = c - (c - a);
    alo = a - ahi;

    c = 134217729.0 * b;
    bhi = c - (c - b);
    blo = b - bhi;

    // compute product error using dekker's formula
    return ((ahi * bhi - p) + ahi * blo + alo * bhi) + alo * blo;
#endif
}

#if defined(SIMD_F128_USE_AVX2)

SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
    return _mm_set_pd(0.0, d);
}

SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
    return _mm_set_pd(lo, hi);
}

SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
    return _mm_xor_pd(x, _mm_set1_pd(-0.0));
}

#define SIMD_F128_PACK(hi, lo) _mm_set_pd((lo), (hi))

#elif defined(SIMD_F128_USE_SSE2)

SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
    return _mm_set_pd(0.0, d);
}

SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
    return _mm_set_pd(lo, hi);
}

SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
    return _mm_xor_pd(x, _mm_set1_pd(-0.0));
}

#define SIMD_F128_PACK(hi, lo) _mm_set_pd((lo), (hi))

#elif defined(SIMD_F128_USE_WASM)

SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
    return wasm_f64x2_make(d, 0.0);
}

SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
    return wasm_f64x2_make(hi, lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
    v128_t neg_mask = wasm_i64x2_const(0x8000000000000000ULL, 0x8000000000000000ULL);
    return wasm_v128_xor(x, neg_mask);
}

#define SIMD_F128_PACK(hi, lo) wasm_f64x2_make((hi), (lo))

#elif defined(SIMD_F128_USE_NEON)

SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
    float64x2_t r = vdupq_n_f64(0.0);
    return vsetq_lane_f64(d, r, 0);
}

SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
    float64x2_t r = vdupq_n_f64(lo);
    return vsetq_lane_f64(hi, r, 0);
}

SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
    return vnegq_f64(x);
}

#define SIMD_F128_PACK(hi_val, lo_val) vsetq_lane_f64((hi_val), vdupq_n_f64((lo_val)), 0)

#else

SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
    simd_f128 res = {d, 0.0};
    return res;
}

SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
    simd_f128 res = {hi, lo};
    return res;
}

SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
    simd_f128 res = {-x.hi, -x.lo};
    return res;
}

#define SIMD_F128_PACK(hi_val, lo_val) ((simd_f128){(hi_val), (lo_val)})

#endif
// modern compilers will vectorize these back to single-lane simd instructions.

SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    double s = ahi + bhi;
    if (__builtin_expect(isinf(s), 0)) return SIMD_F128_PACK(s, 0.0);

    // knuth's two-sum algorithm to compute sum roundoff error e
    double v = s - ahi;
    double e = (ahi - (s - v)) + (bhi - v);

    // combine low components and sum roundoff error, then normalize
    double t = alo + blo + e;
    double final_hi = s + t;
    double final_lo = t - (final_hi - s);

    return SIMD_F128_PACK(final_hi, final_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_add_fast(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    double s = ahi + bhi;
    if (__builtin_expect(isinf(s), 0)) return SIMD_F128_PACK(s, 0.0);

    // fast two-sum algorithm (requires |a| >= |b|)
    double e = bhi - (s - ahi);

    double t = alo + blo + e;
    double final_hi = s + t;
    double final_lo = t - (final_hi - s);

    return SIMD_F128_PACK(final_hi, final_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    double s = ahi - bhi;
    if (__builtin_expect(isinf(s), 0)) return SIMD_F128_PACK(s, 0.0);

    // two-diff algorithm
    double v = s - ahi;
    double e = (ahi - (s - v)) - (bhi + v);

    double t = alo - blo + e;
    double final_hi = s + t;
    double final_lo = t - (final_hi - s);

    return SIMD_F128_PACK(final_hi, final_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_sub_fast(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    double s = ahi - bhi;
    if (__builtin_expect(isinf(s), 0)) return SIMD_F128_PACK(s, 0.0);

    // fast two-diff algorithm (requires |a| >= |b|)
    double e = -bhi - (s - ahi);

    double t = alo - blo + e;
    double final_hi = s + t;
    double final_lo = t - (final_hi - s);

    return SIMD_F128_PACK(final_hi, final_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    // compute base product, estimate exact error, add cross-terms, and normalize
    double p = ahi * bhi;
    if (__builtin_expect(isinf(p), 0)) return SIMD_F128_PACK(p, 0.0);

    double e = simd_f128_exact_mul_err(ahi, bhi, p);
    e += (ahi * blo + alo * bhi);

    double final_hi = p + e;
    double final_lo = e - (final_hi - p);

    return SIMD_F128_PACK(final_hi, final_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
    double ahi, alo, bhi, blo;
    simd_f128_extract(a, &ahi, &alo);
    simd_f128_extract(b, &bhi, &blo);

    // check division by zero
    // check both hi and lo to handle subnormals correctly
    if (bhi == 0.0 && blo == 0.0) {
        double inf_val = ahi / bhi;
        if (ahi == 0.0 && alo == 0.0) inf_val = NAN;
        return SIMD_F128_PACK(inf_val, 0.0);
    }

    // check division by infinity
    if (__builtin_expect(isinf(bhi), 0)) {
        if (isinf(ahi) || isnan(ahi) || isnan(bhi)) {
            return SIMD_F128_PACK(NAN, 0.0);
        }
        double sign = (signbit(ahi) ^ signbit(bhi)) ? -0.0 : 0.0;
        return SIMD_F128_PACK(sign, 0.0);
    }

    // quotient estimation and remainder tracking for scalar division
    double q1 = ahi / bhi;
    double p1 = q1 * bhi;
    double p2 = simd_f128_exact_mul_err(q1, bhi, p1) + q1 * blo;

    double s = ahi - p1;
    double v = s - ahi;
    double e = (ahi - (s - v)) + (-p1 - v);
    double t = alo - p2 + e;

    double rh = s + t;
    double rl = t - (rh - s);
    double q2 = rh / bhi;

    double final_hi = q1 + q2;
    double final_lo = q2 - (final_hi - q1) + rl / bhi;

    return SIMD_F128_PACK(final_hi, final_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
    double xhi, xlo;
    simd_f128_extract(x, &xhi, &xlo);

    // check negative bounds
    if (xhi < 0.0 || (xhi == 0.0 && xlo < 0.0)) {
        return SIMD_F128_PACK(NAN, 0.0);
    }
    if (xhi == 0.0) return x;

    // initial guess is already correct to 1 ulp
    double y = 1.0 / sqrt(xhi);

    double z = xhi * y;
    double zlo = simd_f128_exact_mul_err(xhi, y, z) + xlo * y;

    double est = z * z;
    double estlo = simd_f128_exact_mul_err(z, z, est) + 2.0 * z * zlo;
    double err = (xhi - est) - estlo + xlo;

    double final_hi = z + 0.5 * err * y;
    double final_lo = 0.5 * err * y - (final_hi - z);

    return SIMD_F128_PACK(final_hi, final_lo);
}

// architecture-agnostic inverse square root.
// utilizes the respective hardware-accelerated div and sqrt.
SIMD_F128_INLINE simd_f128 simd_f128_rsqrt(simd_f128 x) {
    simd_f128 one = simd_f128_from_double(1.0);
    return simd_f128_div(one, simd_f128_sqrt(x));
}

#endif
#endif
