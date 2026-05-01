#ifndef SIMD_F128_H
#define SIMD_F128_H

/*
    simd_f128.h -- high-performance 128-bit (double-double) arithmetic for SIMD.

    Project URL: https://github.com/tiw302/simd-f128 (proposed)

    Do this:
        #define SIMD_F128_IMPLEMENTATION
    before you include this file in *one* C or C++ file to create the
    implementation.

    QUICK USAGE:
    ------------
    Initialize:
        simd_f128 a = simd_f128_from_double(3.14159);
        simd_f128 b = simd_f128_from_double(2.71828);

    Arithmetic:
        simd_f128 sum  = simd_f128_add(a, b);
        simd_f128 diff = simd_f128_sub(a, b);
        simd_f128 prod = simd_f128_mul(a, b);

    TECHNICAL BACKGROUND:
    ---------------------
    This library implements "Double-Double" arithmetic, where a high-precision
    number is represented as the sum of two standard 64-bit doubles (hi + lo).
    This provides roughly 106 bits of mantissa (~31 decimal digits), which is
    equivalent to "Quad Precision" (f128).

    Algorithms used:
    - Knuth's TwoSum: Error-free transformation for addition.
    - Dekker's TwoProd: Error-free transformation for multiplication (uses FMA).
    - FastTwoSum: Used for final normalization when |hi| > |lo|.

    SIMD OPTIMIZATION:
    ------------------
    Backend selected automatically at compile time via preprocessor macros:
    - AVX2:     __m128d         (x86_64, Intel Haswell+, AMD Ryzen+)
    - NEON:     float64x2_t     (AArch64: Apple Silicon, Graviton, RPi 4/5, Android)
    - WASM:     v128_t          (WebAssembly + SIMD128 proposal)
    - Scalar:   C99 struct      (ARMv7, RISC-V, MIPS, PowerPC, WASM no-SIMD, etc.)
    No configuration required — the correct path is selected automatically.

    CAVEATS & LIMITATIONS:
    ----------------------
    - FMA Required: For optimal multiplication accuracy, hardware FMA support
      is expected. Most modern x86_64 and ARM64 CPUs provide this.
    - Range: The numerical range is identical to standard IEEE 754 double.
    - Special Values: NaN and Infinity are handled according to standard
      double-precision rules. No special multi-precision handling is applied.
    - Fixed Precision: This library is fixed at 128-bit (double-double).
      It is not a general-purpose arbitrary-precision library.

    LICENSE:
    --------
    MIT License
    Copyright (c) 2026 Jirawat Siripuk
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

#if defined(__AVX2__)
    /*
     * x86_64 with AVX2: Intel Haswell (2013)+, AMD Ryzen (2017)+.
     * Compile with -mavx2 -mfma. Detected automatically by CMake
     * when -DSIMD_F128_AVX2=ON is passed.
     */
    #define SIMD_F128_USE_AVX2
    #include <immintrin.h>

#elif defined(__wasm_simd128__)
    /*
     * WebAssembly with SIMD128 proposal.
     * Emscripten: emcc ... -msimd128
     * Requires Chrome 91+, Firefox 89+, Safari 16.4+, Node.js 16+.
     */
    #define SIMD_F128_USE_WASM
    #include <wasm_simd128.h>

#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_ARCH_ISA_A64)
    /*
     * AArch64 (ARM64): Apple M1/M2/M3, AWS Graviton 2/3, Raspberry Pi 4/5
     * (64-bit OS), Android ARM64 (NDK), iOS (iPhone 5s+), Windows on ARM.
     * float64x2_t NEON is always available on AArch64 — no extra flag needed.
     */
    #define SIMD_F128_USE_NEON
    #include <arm_neon.h>

#elif defined(__arm__) || defined(_M_ARM) || defined(__ARM_ARCH)
    /*
     * ARMv7 / AArch32: Raspberry Pi 2/3 (32-bit OS), older Android,
     * embedded targets (Cortex-A7, A9, A15, A17, A53 in 32-bit mode).
     * NEON on 32-bit ARM only supports float32x4_t — float64x2_t is
     * AArch64-only. Scalar path used here.
     * Tip: compile with -mfpu=neon-vfpv4 -mfloat-abi=hard so fma()
     * maps to the hardware VFMA instruction on VFPv4 cores.
     */
    #define SIMD_F128_USE_SCALAR

#elif defined(__EMSCRIPTEN__)
    /*
     * WebAssembly without SIMD128 (Emscripten without -msimd128).
     * Compatible with all browsers including those that predate SIMD128
     * support. Scalar path provides correct results at reduced throughput.
     */
    #define SIMD_F128_USE_SCALAR

#elif defined(__riscv)
    /*
     * RISC-V (rv32/rv64): SiFive, StarFive VisionFive, SpacemiT K1.
     * The RISC-V Vector extension (RVV) is not yet targeted.
     * Scalar path. fma() maps to hardware FMADD on all standard profiles.
     */
    #define SIMD_F128_USE_SCALAR

#elif defined(__POWERPC__) || defined(__PPC__) || defined(__ppc__)
    /*
     * PowerPC / POWER (IBM): AIX, Linux ppc64le (Talos II, etc.).
     * VMX/VSX intrinsics not yet targeted. Scalar path.
     */
    #define SIMD_F128_USE_SCALAR

#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
    /*
     * MIPS (32/64-bit): embedded SoCs, older routers, MIPS64 servers.
     * Scalar path.
     */
    #define SIMD_F128_USE_SCALAR

#else
    /* All other architectures: LoongArch, s390x, SPARC, etc. */
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
 * provides ~31 decimal digits of precision
 */

#if defined(SIMD_F128_USE_AVX2)
    /* [hi, lo] -> Lane 1: high, Lane 0: low */
    typedef __m128d simd_f128;
#elif defined(SIMD_F128_USE_WASM)
    /* [hi, lo] -> Lane 1: high, Lane 0: low */
    typedef v128_t simd_f128;
#elif defined(SIMD_F128_USE_NEON)
    /* [lo, hi] -> Lane 0: low, Lane 1: high */
    typedef float64x2_t simd_f128;
#else
    // Scalar fallback
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

// Initialization
static inline simd_f128 simd_f128_from_double(double d);

// Arithmetic
static inline simd_f128 simd_f128_add(simd_f128 a, simd_f128 b);
static inline simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b);
static inline simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b);

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

    static inline simd_f128 simd_f128_from_double(double d) {
        return _mm_set_pd(d, 0.0); /* Lane 1 = high, Lane 0 = low */
    }

    /* FastTwoSum: requires |a| >= |b|. a+b = s+e */
    static inline void _simd_fast_two_sum(double a, double b, double* s, double* e) {
        *s = a + b;
        *e = b - (*s - a);
    }

    static inline void _simd_two_sum(double a, double b, double* s, double* e) {
        /*
          Knuth's TwoSum: Computes s = a + b exactly.
          The floating point result 's' carries the main sum.
          The error 'e' is computed by subtracting the components of 's' from 'a' and 'b'.
          Because this is an Error-Free Transformation (EFT), a + b == s + e exactly.
        */
        *s = a + b;
        double v = *s - a;
        *e = (a - (*s - v)) + (b - v);
    }

    static inline void _simd_two_prod(double a, double b, double* p, double* e) {
        /*
          Dekker's TwoProd (via FMA): Computes p = a * b exactly.
          The error 'e' is calculated by (a * b) - p in infinite precision.
          Standard hardware FMA (Fused Multiply-Add) makes this extremely fast.
        */
        *p = a * b;
        *e = fma(a, b, -(*p));
    }

    static inline simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        /* extract hi/lo components */
        double ahi = _mm_cvtsd_f64(_mm_unpackhi_pd(a, a));
        double alo = _mm_cvtsd_f64(a);
        double bhi = _mm_cvtsd_f64(_mm_unpackhi_pd(b, b));
        double blo = _mm_cvtsd_f64(b);

        /*
           Double-Double Addition (hi + lo):
           1. Exact sum of high parts: ahi + bhi = s + e
           2. Sum low parts and error: t = alo + blo + e
           3. Final normalization: FastTwoSum(s, t) -> ensure |hi| > |lo|
        */
        double s, e, t;
        _simd_two_sum(ahi, bhi, &s, &e);
        t = alo + blo + e;

        double final_hi, final_lo;
        _simd_fast_two_sum(s, t, &final_hi, &final_lo);
        return _mm_set_pd(final_hi, final_lo);
    }

    static inline simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        /* negate b and add */
        __m128d neg_mask = _mm_set1_pd(-0.0);
        simd_f128 neg_b = _mm_xor_pd(b, neg_mask);
        return simd_f128_add(a, neg_b);
    }

    static inline simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi = _mm_cvtsd_f64(_mm_unpackhi_pd(a, a));
        double alo = _mm_cvtsd_f64(a);
        double bhi = _mm_cvtsd_f64(_mm_unpackhi_pd(b, b));
        double blo = _mm_cvtsd_f64(b);

        /*
           Double-Double Multiplication:
           1. Compute exact product (p) and error (e) of (ahi * bhi) using FMA
           2. Add cross-products (ahi * blo) and (alo * bhi) to the error
           3. The (alo * blo) term is usually discarded as it's below the 128-bit precision limit
        */
        double p, e;
        _simd_two_prod(ahi, bhi, &p, &e);
        e += (ahi * blo) + (alo * bhi);

        double final_hi, final_lo;
        _simd_fast_two_sum(p, e, &final_hi, &final_lo);
        return _mm_set_pd(final_hi, final_lo);
    }

#elif defined(SIMD_F128_USE_WASM)

    static inline simd_f128 simd_f128_from_double(double d) {
        return wasm_f64x2_make(0.0, d); /* Lane 0 = low, Lane 1 = high */
    }

    /* FastTwoSum: requires |a| >= |b|. a+b = s+e */
    static inline void _simd_fast_two_sum(double a, double b, double* s, double* e) {

        *s = a + b; *e = b - (*s - a);
    }
    /* TwoSum: Knuth algorithm. a+b = s+e */
    static inline void _simd_two_sum(double a, double b, double* s, double* e) {

        *s = a + b; double v = *s - a; *e = (a - (*s - v)) + (b - v);
    }
    /* TwoProd: a*b = p+e, uses FMA if available */
    static inline void _simd_two_prod(double a, double b, double* p, double* e) {

        *p = a * b; *e = fma(a, b, -(*p));
    }

    static inline simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double alo = wasm_f64x2_extract_lane(a, 0);
        double ahi = wasm_f64x2_extract_lane(a, 1);
        double blo = wasm_f64x2_extract_lane(b, 0);
        double bhi = wasm_f64x2_extract_lane(b, 1);

        double s, e, t1, t2;
        _simd_two_sum(ahi, bhi, &s, &e);
        t1 = alo + blo;
        t2 = e + t1;

        double final_hi, final_lo;
        _simd_fast_two_sum(s, t2, &final_hi, &final_lo);
        return wasm_f64x2_make(final_lo, final_hi);
    }

    static inline simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        v128_t neg_mask = wasm_i64x2_const(0x8000000000000000ULL, 0x8000000000000000ULL);
        simd_f128 neg_b = wasm_v128_xor(b, neg_mask);
        return simd_f128_add(a, neg_b);
    }

    static inline simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double alo = wasm_f64x2_extract_lane(a, 0);
        double ahi = wasm_f64x2_extract_lane(a, 1);
        double blo = wasm_f64x2_extract_lane(b, 0);
        double bhi = wasm_f64x2_extract_lane(b, 1);

        double p, e;
        _simd_two_prod(ahi, bhi, &p, &e);
        e += (ahi * blo) + (alo * bhi);

        double final_hi, final_lo;
        _simd_fast_two_sum(p, e, &final_hi, &final_lo);
        return wasm_f64x2_make(final_lo, final_hi);
    }

#elif defined(SIMD_F128_USE_NEON)

    static inline simd_f128 simd_f128_from_double(double d) {
        /* lane 1 = hi = d, lane 0 = lo = 0.0 */
        float64x2_t r = vdupq_n_f64(0.0);
        return vsetq_lane_f64(d, r, 1);
    }

    static inline void _simd_fast_two_sum(double a, double b, double* s, double* e) {
        *s = a + b; *e = b - (*s - a);
    }
    static inline void _simd_two_sum(double a, double b, double* s, double* e) {
        *s = a + b; double v = *s - a; *e = (a - (*s - v)) + (b - v);
    }
    static inline void _simd_two_prod(double a, double b, double* p, double* e) {
        *p = a * b; *e = fma(a, b, -(*p));
    }

    static inline simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 1);
        double alo = vgetq_lane_f64(a, 0);
        double bhi = vgetq_lane_f64(b, 1);
        double blo = vgetq_lane_f64(b, 0);

        double s, e, t;
        _simd_two_sum(ahi, bhi, &s, &e);
        t = alo + blo + e;

        double final_hi, final_lo;
        _simd_fast_two_sum(s, t, &final_hi, &final_lo);

        float64x2_t r = vdupq_n_f64(0.0);
        r = vsetq_lane_f64(final_hi, r, 1);
        r = vsetq_lane_f64(final_lo, r, 0);
        return r;
    }

    static inline simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        /* negate both lanes of b */
        simd_f128 neg_b = vnegq_f64(b);
        return simd_f128_add(a, neg_b);
    }

    static inline simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 1);
        double alo = vgetq_lane_f64(a, 0);
        double bhi = vgetq_lane_f64(b, 1);
        double blo = vgetq_lane_f64(b, 0);

        double p, e;
        _simd_two_prod(ahi, bhi, &p, &e);
        e += (ahi * blo) + (alo * bhi);

        double final_hi, final_lo;
        _simd_fast_two_sum(p, e, &final_hi, &final_lo);

        float64x2_t r = vdupq_n_f64(0.0);
        r = vsetq_lane_f64(final_hi, r, 1);
        r = vsetq_lane_f64(final_lo, r, 0);
        return r;
    }

#else
    static inline simd_f128 simd_f128_from_double(double d) {
        simd_f128 res;
        res.hi = d; res.lo = 0.0;
        return res;
    }

    static inline void _simd_fast_two_sum(double a, double b, double* s, double* e) {
        *s = a + b; *e = b - (*s - a);
    }
    static inline void _simd_two_sum(double a, double b, double* s, double* e) {
        *s = a + b; double v = *s - a; *e = (a - (*s - v)) + (b - v);
    }
    /* TwoProd: a*b = p+e, uses FMA if available */
    static inline void _simd_two_prod(double a, double b, double* p, double* e) {

        *p = a * b; *e = fma(a, b, -(*p));
    }

    static inline simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double s, e, t1, t2;
        _simd_two_sum(a.hi, b.hi, &s, &e);
        t1 = a.lo + b.lo;
        t2 = e + t1;
        simd_f128 res;
        _simd_fast_two_sum(s, t2, &res.hi, &res.lo);
        return res;
    }

    static inline simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        b.hi = -b.hi; b.lo = -b.lo;
        return simd_f128_add(a, b);
    }

    static inline simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double p, e;
        _simd_two_prod(a.hi, b.hi, &p, &e);
        e += (a.hi * b.lo) + (a.lo * b.hi);
        simd_f128 res;
        _simd_fast_two_sum(p, e, &res.hi, &res.lo);
        return res;
    }
#endif

#endif // SIMD_F128_IMPLEMENTATION
#endif // SIMD_F128_H
