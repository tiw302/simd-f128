// updated 2026-06-14
// spdx-license-identifier: mit
// copyright (c) 2026 jirawat siripuk

/*
 *       /\_/\
 *      ( -.- )  <-- "precision: 31 digits. developer sanity: 0 digits."
 *     ==  Y  ==
 *      (  .  )
 *      (  .  )
 */

#ifndef SIMD_F128_H
#define SIMD_F128_H

/*
 * simd_f128.h -- high-performance 128-bit (double-double) arithmetic for simd.
 * project url: https://github.com/tiw302/simd-f128
 * do this:
 * #define SIMD_F128_IMPLEMENTATION
 * before you include this file in *one* c or c++ file to create the
 * implementation.
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 * simd optimization:
 * ------------------
 * we've got backends for pretty much everything:
 * - avx2:     x86_64 modern (haswell+, ryzen+)
 * - sse2:     x86_64 older or low-power
 * - neon:     arm64 (apple silicon, graviton, android)
 * - wasm:     webassembly with simd128
 * - scalar:   fallback for everything else (risc-v, ppc, etc.)
 * license:
 * --------
 * mit license
 * copyright (c) 2026 jirawat siripuk
 * */

#include <stdint.h>
#include <math.h>

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

// msvc doesn't support gcc builtins like __builtin_expect, so we define a fallback macro
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__builtin_expect)
#define __builtin_expect(x, y) (x)
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

// gpu (cuda/hip) support
#if defined(__CUDACC__) || defined(__HIPCC__)
    #define SIMD_F128_DEVICE __device__ __host__
#else
    #define SIMD_F128_DEVICE
#endif

/*
 * we use always_inline to make sure there's no function call overhead.
 * for double-double arithmetic, the overhead of a function call can
 * be significant compared to the actual math.
 */
#if defined(__CUDACC__) || defined(__HIPCC__)
    #define SIMD_F128_INLINE SIMD_F128_DEVICE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define SIMD_F128_INLINE SIMD_F128_DEVICE static __forceinline
#else
    #define SIMD_F128_INLINE SIMD_F128_DEVICE static inline __attribute__((always_inline))
#endif

/* initialization routines:
 * from_double creates a 128-bit number from a standard 64-bit double by
 * placing it in the high component and zeroing the low component.
 * from_hi_lo allows precise manual construction of a double-double. */
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
        // prevent overflow in split for huge numbers by reciprocal scaling
        if (__builtin_expect(fabs(a) > 6.7e299 && fabs(b) < 1.0, 0)) {
            a *= 3.7252902984619140625e-09; // 2^-28
            b *= 268435456.0;               // 2^28
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
        // initialize double-double value from a single double (hi = d, lo = 0.0)
        return _mm_set_pd(0.0, d);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
        // initialize double-double value from high and low parts
        return _mm_set_pd(lo, hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
        // negate both hi and lo parts by flipping the sign bit using bitwise xor
        return _mm_xor_pd(x, _mm_set1_pd(-0.0));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        // knuth's two-sum in full intrinsic form — stays in simd registers.
        // hi is in the low lane of each __m128d; lo is in the high lane.
        __m128d ahi = _mm_unpacklo_pd(a, a);  // broadcast hi of a
        __m128d bhi = _mm_unpacklo_pd(b, b);  // broadcast hi of b
        __m128d s   = _mm_add_sd(ahi, bhi);

        // _mm_cvtsd_f64: register-to-register move, zero latency, safe under -ffast-math.
        // isinf guard prevents nan from (inf-inf) in the error term below.
        if (__builtin_expect(isinf(_mm_cvtsd_f64(s)), 0))
            return _mm_unpacklo_pd(s, _mm_setzero_pd());

        __m128d v   = _mm_sub_sd(s, ahi);
        __m128d e   = _mm_add_sd(
                          _mm_sub_sd(ahi, _mm_sub_sd(s, v)),
                          _mm_sub_sd(bhi, v));
        __m128d alo = _mm_unpackhi_pd(a, a);  // broadcast lo of a
        __m128d blo = _mm_unpackhi_pd(b, b);  // broadcast lo of b
        __m128d t   = _mm_add_sd(_mm_add_sd(alo, blo), e);
        __m128d fh  = _mm_add_sd(s, t);
        __m128d fl  = _mm_sub_sd(t, _mm_sub_sd(fh, s));
        return _mm_unpacklo_pd(fh, fl);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        // two-diff: direct subtraction analog of knuth's two-sum.
        // avoids the neg + add chain and the extra isinf check on negation.
        __m128d ahi = _mm_unpacklo_pd(a, a);
        __m128d bhi = _mm_unpacklo_pd(b, b);
        __m128d s   = _mm_sub_sd(ahi, bhi);

        if (__builtin_expect(isinf(_mm_cvtsd_f64(s)), 0))
            return _mm_unpacklo_pd(s, _mm_setzero_pd());

        __m128d v   = _mm_sub_sd(s, ahi);
        __m128d e   = _mm_sub_sd(
                          _mm_sub_sd(ahi, _mm_sub_sd(s, v)),
                          _mm_add_sd(bhi, v));
        __m128d alo = _mm_unpackhi_pd(a, a);
        __m128d blo = _mm_unpackhi_pd(b, b);
        __m128d t   = _mm_add_sd(_mm_sub_sd(alo, blo), e);
        __m128d fh  = _mm_add_sd(s, t);
        __m128d fl  = _mm_sub_sd(t, _mm_sub_sd(fh, s));
        return _mm_unpacklo_pd(fh, fl);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        // multiplication using hardware fused multiply-accumulate (fma):
        // fma computes (a * b) + c with only a single rounding step.
        // therefore, fma(a, b, - (a * b)) computes the exact roundoff error.
        __m128d ahi = _mm_unpacklo_pd(a, a);
        __m128d bhi = _mm_unpacklo_pd(b, b);
        __m128d p = _mm_mul_sd(ahi, bhi);

        // overflow guard: return early on infinite product to avoid nan propagation
        if (__builtin_expect(isinf(_mm_cvtsd_f64(p)), 0))
            return _mm_unpacklo_pd(p, _mm_setzero_pd());

        __m128d e = _mm_fmsub_sd(ahi, bhi, p);

        // integrate the cross-terms from the low parts
        __m128d alo = _mm_unpackhi_pd(a, a);
        __m128d blo = _mm_unpackhi_pd(b, b);
        __m128d c1 = _mm_mul_sd(ahi, blo);
        __m128d c2 = _mm_mul_sd(alo, bhi);
        e = _mm_add_sd(e, _mm_add_sd(c1, c2));

        // normalize the final hi and lo parts
        __m128d final_hi = _mm_add_sd(p, e);
        __m128d final_lo = _mm_sub_sd(e, _mm_sub_sd(final_hi, p));

        return _mm_unpacklo_pd(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        // check for division by zero to follow ieee-754 sign rules
        // check both hi and lo to handle subnormals correctly
        if (bhi == 0.0 && blo == 0.0) {
            double inf_val = ahi / bhi;
            if (ahi == 0.0 && alo == 0.0) inf_val = NAN;
            return _mm_set_pd(0.0, inf_val);
        }

        // check division by infinity
        if (__builtin_expect(isinf(bhi), 0)) {
            if (isinf(ahi) || isnan(ahi) || isnan(bhi)) {
                return _mm_set_pd(0.0, NAN);
            }
            double sign = (signbit(ahi) ^ signbit(bhi)) ? -0.0 : 0.0;
            return _mm_set_pd(0.0, sign);
        }

        // double-precision division:
        // compute initial quotient guess q1, then compute the exact product q1 * b
        double q1 = ahi / bhi;
        double p1 = q1 * bhi;
        double p2 = fma(q1, bhi, -p1) + q1 * blo;

        // compute the exact remainder of the division step
        double s = ahi - p1;
        double v = s - ahi;
        double e = (ahi - (s - v)) + (-p1 - v);
        double t = alo - p2 + e;

        // compute the correction term q2 and normalize
        double rh = s + t;
        double rl = t - (rh - s);
        double q2 = rh / bhi;

        double final_hi = q1 + q2;
        double final_lo = q2 - (final_hi - q1) + rl / bhi;

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi, xlo;
        simd_f128_extract(x, &xhi, &xlo);

        // check for invalid negative inputs
        if (xhi < 0.0 || (xhi == 0.0 && xlo < 0.0)) {
            return _mm_set_pd(0.0, NAN);
        }
        if (xhi == 0.0) return x;

        // initial hardware guess for 1/sqrt(xhi) is already correct to 1 ulp
        double y = 1.0 / sqrt(xhi);

        // compute initial guess for sqrt(x) -> z = x * y
        double z = xhi * y;
        double zlo = fma(xhi, y, -z) + xlo * y;

        // refine and compute error of z^2 vs x
        double est = z * z;
        double estlo = fma(z, z, -est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        // apply the correction term
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
        // load high part into lower lane, set low part to 0.0
        return _mm_set_pd(0.0, d);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
        // load hi and lo parts into the sse2 register
        return _mm_set_pd(lo, hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
        // negate hi and lo parts by flipping their sign bits using bitwise xor
        return _mm_xor_pd(x, _mm_set1_pd(-0.0));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        // compute double sum
        double s = ahi + bhi;

        // overflow guard: check for infinite sum to avoid generating nan
        if (isinf(s)) return _mm_set_pd(0.0, s);

        // knuth's two-sum algorithm to compute roundoff error e
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);

        // accumulate low parts and error, then normalize
        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double s = ahi - bhi;
        if (isinf(s)) return _mm_set_pd(0.0, s);

        double v = s - ahi;
        double e = (ahi - (s - v)) - (bhi + v);
        double t = alo - blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        // compute primary product and get exact error using dekker's fallback
        double p = ahi * bhi;

        if (__builtin_expect(isinf(p), 0))
            return _mm_set_pd(0.0, p);

        double e = simd_f128_exact_mul_err(ahi, bhi, p);
        e += (ahi * blo + alo * bhi);

        // normalize the final hi/lo parts
        double final_hi = p + e;
        double final_lo = e - (final_hi - p);

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        // check for division by zero
        // check both hi and lo to handle subnormals correctly
        if (bhi == 0.0 && blo == 0.0) {
            double inf_val = ahi / bhi;
            if (ahi == 0.0 && alo == 0.0) inf_val = NAN;
            return _mm_set_pd(0.0, inf_val);
        }

        // check division by infinity
        if (__builtin_expect(isinf(bhi), 0)) {
            if (isinf(ahi) || isnan(ahi) || isnan(bhi)) {
                return _mm_set_pd(0.0, NAN);
            }
            double sign = (signbit(ahi) ^ signbit(bhi)) ? -0.0 : 0.0;
            return _mm_set_pd(0.0, sign);
        }

        // division using initial quotient estimation and remainder tracking
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

        return _mm_set_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi, xlo;
        simd_f128_extract(x, &xhi, &xlo);

        // check negative bounds
        if (xhi < 0.0 || (xhi == 0.0 && xlo < 0.0)) {
            return _mm_set_pd(0.0, NAN);
        }
        if (xhi == 0.0) return x;

        // initial guess for 1/sqrt(x) is already correct to 1 ulp
        double y = 1.0 / sqrt(xhi);

        double z = xhi * y;
        double zlo = simd_f128_exact_mul_err(xhi, y, z) + xlo * y;

        double est = z * z;
        double estlo = simd_f128_exact_mul_err(z, z, est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        return _mm_set_pd(final_lo, final_hi);
    }

#elif defined(SIMD_F128_USE_WASM)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        // load high part into lane 0, set lane 1 (low part) to 0.0
        return wasm_f64x2_make(d, 0.0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
        // construct webassembly v128 register directly from hi and lo parts
        return wasm_f64x2_make(hi, lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
        // flip sign bits using a bitwise xor mask for double precision lanes
        v128_t neg_mask = wasm_i64x2_const(0x8000000000000000ULL, 0x8000000000000000ULL);
        return wasm_v128_xor(x, neg_mask);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double s = ahi + bhi;

        // check for overflow before calculating error
        if (isinf(s)) return wasm_f64x2_make(s, 0.0);

        // knuth's two-sum algorithm
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);

        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        double s = ahi - bhi;
        if (isinf(s)) return wasm_f64x2_make(s, 0.0);

        double v = s - ahi;
        double e = (ahi - (s - v)) - (bhi + v);
        double t = alo - blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        // double multiplication with error correction
        double p = ahi * bhi;
        if (__builtin_expect(isinf(p), 0))
            return wasm_f64x2_make(p, 0.0);

        double e = simd_f128_exact_mul_err(ahi, bhi, p);
        e += (ahi * blo + alo * bhi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);

        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi, alo, bhi, blo;
        simd_f128_extract(a, &ahi, &alo);
        simd_f128_extract(b, &bhi, &blo);

        // check for division by zero
        // check both hi and lo to handle subnormals correctly
        if (bhi == 0.0 && blo == 0.0) {
            double inf_val = ahi / bhi;
            if (ahi == 0.0 && alo == 0.0) inf_val = NAN;
            return wasm_f64x2_make(inf_val, 0.0);
        }

        // check division by infinity
        if (__builtin_expect(isinf(bhi), 0)) {
            if (isinf(ahi) || isnan(ahi) || isnan(bhi)) {
                return wasm_f64x2_make(NAN, 0.0);
            }
            double sign = (signbit(ahi) ^ signbit(bhi)) ? -0.0 : 0.0;
            return wasm_f64x2_make(sign, 0.0);
        }

        // compute initial quotient and exact remainder
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

        return wasm_f64x2_make(final_hi, final_lo);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi, xlo;
        simd_f128_extract(x, &xhi, &xlo);

        // negative boundaries check
        if (xhi < 0.0 || (xhi == 0.0 && xlo < 0.0)) {
            return wasm_f64x2_make(NAN, 0.0);
        }
        if (xhi == 0.0) return x;

        // initial guess for 1/sqrt(x) is already correct to 1 ulp
        double y = 1.0 / sqrt(xhi);

        double z = xhi * y;
        double zlo = simd_f128_exact_mul_err(xhi, y, z) + xlo * y;

        double est = z * z;
        double estlo = simd_f128_exact_mul_err(z, z, est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        return wasm_f64x2_make(final_hi, final_lo);
    }

#elif defined(SIMD_F128_USE_NEON)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        // load double into lane 0 of arm64 neon float64x2 register
        float64x2_t r = vdupq_n_f64(0.0);
        return vsetq_lane_f64(d, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
        // hi loaded in index 0, lo loaded in index 1 for consistent representation
        float64x2_t r = vdupq_n_f64(lo);
        return vsetq_lane_f64(hi, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
        // negate both lanes using arm64 neon vector negate instruction
        return vnegq_f64(x);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        // extract hi and lo components from neon registers
        double ahi = vgetq_lane_f64(a, 0);
        double alo = vgetq_lane_f64(a, 1);
        double bhi = vgetq_lane_f64(b, 0);
        double blo = vgetq_lane_f64(b, 1);

        double s = ahi + bhi;

        // check for overflow to skip error tracking
        if (isinf(s)) {
            float64x2_t r = vdupq_n_f64(0.0);
            return vsetq_lane_f64(s, r, 0);
        }

        // knuth's two-sum algorithm
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);

        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        float64x2_t r = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 0);
        double alo = vgetq_lane_f64(a, 1);
        double bhi = vgetq_lane_f64(b, 0);
        double blo = vgetq_lane_f64(b, 1);

        double s = ahi - bhi;
        if (isinf(s)) {
            float64x2_t r = vdupq_n_f64(0.0);
            return vsetq_lane_f64(s, r, 0);
        }

        double v = s - ahi;
        double e = (ahi - (s - v)) - (bhi + v);
        double t = alo - blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        float64x2_t r = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double xhi = vgetq_lane_f64(a, 0);
        double xlo = vgetq_lane_f64(a, 1);
        double yhi = vgetq_lane_f64(b, 0);
        double ylo = vgetq_lane_f64(b, 1);

        double z = xhi * yhi;
        if (__builtin_expect(isinf(z), 0)) {
            float64x2_t r_res = vdupq_n_f64(0.0);
            return vsetq_lane_f64(z, r_res, 0);
        }

        double e = simd_f128_exact_mul_err(xhi, yhi, z);
        e += (xhi * ylo + xlo * yhi);

        double final_hi = z + e;
        double final_lo = e - (final_hi - z);

        float64x2_t r = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 0);
        double alo = vgetq_lane_f64(a, 1);
        double bhi = vgetq_lane_f64(b, 0);
        double blo = vgetq_lane_f64(b, 1);

        // division by zero check
        // check both hi and lo to handle subnormals correctly
        if (bhi == 0.0 && blo == 0.0) {
            double inf_val = ahi / bhi;
            if (ahi == 0.0 && alo == 0.0) inf_val = NAN;
            float64x2_t r_res = vdupq_n_f64(0.0);
            return vsetq_lane_f64(inf_val, r_res, 0);
        }

        // check division by infinity
        if (__builtin_expect(isinf(bhi), 0)) {
            float64x2_t r_res = vdupq_n_f64(0.0);
            if (isinf(ahi) || isnan(ahi) || isnan(bhi)) {
                return vsetq_lane_f64(NAN, r_res, 0);
            }
            double sign = (signbit(ahi) ^ signbit(bhi)) ? -0.0 : 0.0;
            return vsetq_lane_f64(sign, r_res, 0);
        }

        // compute initial quotient and refine via division remainder
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

        float64x2_t r_res = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r_res, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        double xhi = vgetq_lane_f64(x, 0);
        double xlo = vgetq_lane_f64(x, 1);

        // negative values check
        if (xhi < 0.0 || (xhi == 0.0 && xlo < 0.0)) {
            float64x2_t r_res = vdupq_n_f64(0.0);
            return vsetq_lane_f64(NAN, r_res, 0);
        }
        if (xhi == 0.0) return x;

        // initial guess for 1/sqrt(x) is already correct to 1 ulp
        double y = 1.0 / sqrt(xhi);

        double z = xhi * y;
        double zlo = simd_f128_exact_mul_err(xhi, y, z) + xlo * y;

        double est = z * z;
        double estlo = simd_f128_exact_mul_err(z, z, est) + 2.0 * z * zlo;
        double err = (xhi - est) - estlo + xlo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        float64x2_t r_res = vdupq_n_f64(final_lo);
        return vsetq_lane_f64(final_hi, r_res, 0);
    }

#else
    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        // scalar fallback constructor: set hi = d, lo = 0.0
        simd_f128 res = {d, 0.0};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_from_hi_lo(double hi, double lo) {
        // scalar fallback constructor from hi and lo parts
        simd_f128 res = {hi, lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_neg(simd_f128 x) {
        // scalar fallback negation: negate both fields
        simd_f128 res = {-x.hi, -x.lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double s = a.hi + b.hi;

        // overflow guard: return early on infinite sum to avoid nan propagation
        if (isinf(s)) {
            simd_f128 res = {s, 0.0};
            return res;
        }

        // knuth's two-sum algorithm to compute sum roundoff error e
        double v = s - a.hi;
        double e = (a.hi - (s - v)) + (b.hi - v);

        // combine low components and sum roundoff error, then normalize
        double t = a.lo + b.lo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        double s = a.hi - b.hi;
        if (isinf(s)) {
            simd_f128 res = {s, 0.0};
            return res;
        }
        double v = s - a.hi;
        double e = (a.hi - (s - v)) - (b.hi + v);
        double t = a.lo - b.lo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);
        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        // scalar double-double multiplication:
        // compute base product, estimate its exact error, add cross-terms, and normalize
        double p = a.hi * b.hi;
        if (__builtin_expect(isinf(p), 0)) {
            simd_f128 res = {p, 0.0};
            return res;
        }

        double e = simd_f128_exact_mul_err(a.hi, b.hi, p);
        e += (a.hi * b.lo + a.lo * b.hi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);

        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_div(simd_f128 a, simd_f128 b) {
        // check division by zero
        // check both hi and lo to handle subnormals correctly
        if (b.hi == 0.0 && b.lo == 0.0) {
            double inf_val = a.hi / b.hi;
            if (a.hi == 0.0 && a.lo == 0.0) inf_val = NAN;
            simd_f128 res = {inf_val, 0.0};
            return res;
        }

        // check division by infinity
        if (__builtin_expect(isinf(b.hi), 0)) {
            if (isinf(a.hi) || isnan(a.hi) || isnan(b.hi)) {
                simd_f128 res = {NAN, 0.0};
                return res;
            }
            double sign = (signbit(a.hi) ^ signbit(b.hi)) ? -0.0 : 0.0;
            simd_f128 res = {sign, 0.0};
            return res;
        }

        // quotient estimation and remainder tracking for scalar division
        double q1 = a.hi / b.hi;
        double p1 = q1 * b.hi;
        double p2 = simd_f128_exact_mul_err(q1, b.hi, p1) + q1 * b.lo;

        double s = a.hi - p1;
        double v = s - a.hi;
        double e = (a.hi - (s - v)) + (-p1 - v);
        double t = a.lo - p2 + e;

        double rh = s + t;
        double rl = t - (rh - s);
        double q2 = rh / b.hi;

        double final_hi = q1 + q2;
        double final_lo = q2 - (final_hi - q1) + rl / b.hi;

        simd_f128 res = {final_hi, final_lo};
        return res;
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sqrt(simd_f128 x) {
        // check negative bounds
        if (x.hi < 0.0 || (x.hi == 0.0 && x.lo < 0.0)) {
            simd_f128 res = {NAN, 0.0};
            return res;
        }
        if (x.hi == 0.0) return x;

        // initial guess is already correct to 1 ulp
        double y = 1.0 / sqrt(x.hi);

        double z = x.hi * y;
        double zlo = simd_f128_exact_mul_err(x.hi, y, z) + x.lo * y;

        double est = z * z;
        double estlo = simd_f128_exact_mul_err(z, z, est) + 2.0 * z * zlo;
        double err = (x.hi - est) - estlo + x.lo;

        double final_hi = z + 0.5 * err * y;
        double final_lo = 0.5 * err * y - (final_hi - z);

        simd_f128 res = {final_hi, final_lo};
        return res;
    }

#endif

    /*
     * architecture-agnostic inverse square root.
     * utilizes the respective hardware-accelerated div and sqrt.
     */
    SIMD_F128_INLINE simd_f128 simd_f128_rsqrt(simd_f128 x) {
        simd_f128 one = simd_f128_from_double(1.0);
        return simd_f128_div(one, simd_f128_sqrt(x));
    }

#endif // simd_f128_implementation
#endif // simd_f128_h
