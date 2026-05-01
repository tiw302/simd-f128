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

#define SIMD_F128_INLINE static inline __attribute__((always_inline))

// Initialization
SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d);

// Arithmetic
SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b);
SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b);

// Extraction
SIMD_F128_INLINE void simd_f128_extract(simd_f128 x, double* hi, double* lo) {
#if defined(SIMD_F128_USE_AVX2)
    *hi = _mm_cvtsd_f64(_mm_unpackhi_pd(x, x));
    *lo = _mm_cvtsd_f64(x);
#elif defined(SIMD_F128_USE_WASM)
    *hi = wasm_f64x2_extract_lane(x, 1);
    *lo = wasm_f64x2_extract_lane(x, 0);
#elif defined(SIMD_F128_USE_NEON)
    *hi = vgetq_lane_f64(x, 1);
    *lo = vgetq_lane_f64(x, 0);
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
        return _mm_set_pd(d, 0.0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        /* XMM Pure Scalar Pipeline Operations */
        __m128d ahi = _mm_unpackhi_pd(a, a);
        __m128d bhi = _mm_unpackhi_pd(b, b);
        
        __m128d s = _mm_add_sd(ahi, bhi);
        __m128d v = _mm_sub_sd(s, ahi);
        __m128d e = _mm_add_sd(_mm_sub_sd(ahi, _mm_sub_sd(s, v)), _mm_sub_sd(bhi, v));

        __m128d t = _mm_add_sd(_mm_add_sd(a, b), e);

        __m128d final_hi = _mm_add_sd(s, t);
        __m128d final_lo = _mm_sub_sd(t, _mm_sub_sd(final_hi, s));

        return _mm_unpacklo_pd(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        return simd_f128_add(a, _mm_xor_pd(b, _mm_set1_pd(-0.0)));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        __m128d ahi = _mm_unpackhi_pd(a, a);
        __m128d bhi = _mm_unpackhi_pd(b, b);
        
        __m128d p = _mm_mul_sd(ahi, bhi);
        /* Hardware FMA3 bypasses math.h completely */
        __m128d e = _mm_fmsub_sd(ahi, bhi, p);
        
        __m128d c1 = _mm_mul_sd(ahi, b);
        __m128d c2 = _mm_mul_sd(a, bhi);
        e = _mm_add_sd(e, _mm_add_sd(c1, c2));

        __m128d final_hi = _mm_add_sd(p, e);
        __m128d final_lo = _mm_sub_sd(e, _mm_sub_sd(final_hi, p));

        return _mm_unpacklo_pd(final_lo, final_hi);
    }

#elif defined(SIMD_F128_USE_WASM)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        return wasm_f64x2_make(0.0, d);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi = wasm_f64x2_extract_lane(a, 1);
        double alo = wasm_f64x2_extract_lane(a, 0);
        double bhi = wasm_f64x2_extract_lane(b, 1);
        double blo = wasm_f64x2_extract_lane(b, 0);

        double s = ahi + bhi;
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);
        
        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        return wasm_f64x2_make(final_lo, final_hi);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        v128_t neg_mask = wasm_i64x2_const(0x8000000000000000ULL, 0x8000000000000000ULL);
        return simd_f128_add(a, wasm_v128_xor(b, neg_mask));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi = wasm_f64x2_extract_lane(a, 1);
        double alo = wasm_f64x2_extract_lane(a, 0);
        double bhi = wasm_f64x2_extract_lane(b, 1);
        double blo = wasm_f64x2_extract_lane(b, 0);

        double p = ahi * bhi;
        double e = fma(ahi, bhi, -p);
        e += fma(ahi, blo, alo * bhi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);
        
        return wasm_f64x2_make(final_lo, final_hi);
    }

#elif defined(SIMD_F128_USE_NEON)

    SIMD_F128_INLINE simd_f128 simd_f128_from_double(double d) {
        float64x2_t r = vdupq_n_f64(0.0);
        return vsetq_lane_f64(d, r, 1);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_add(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 1);
        double alo = vgetq_lane_f64(a, 0);
        double bhi = vgetq_lane_f64(b, 1);
        double blo = vgetq_lane_f64(b, 0);

        double s = ahi + bhi;
        double v = s - ahi;
        double e = (ahi - (s - v)) + (bhi - v);
        
        double t = alo + blo + e;
        double final_hi = s + t;
        double final_lo = t - (final_hi - s);

        float64x2_t r = vdupq_n_f64(0.0);
        r = vsetq_lane_f64(final_hi, r, 1);
        return vsetq_lane_f64(final_lo, r, 0);
    }

    SIMD_F128_INLINE simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b) {
        return simd_f128_add(a, vnegq_f64(b));
    }

    SIMD_F128_INLINE simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b) {
        double ahi = vgetq_lane_f64(a, 1);
        double alo = vgetq_lane_f64(a, 0);
        double bhi = vgetq_lane_f64(b, 1);
        double blo = vgetq_lane_f64(b, 0);

        double p = ahi * bhi;
        double e = fma(ahi, bhi, -p);
        e += fma(ahi, blo, alo * bhi);

        double final_hi = p + e;
        double final_lo = e - (final_hi - p);

        float64x2_t r = vdupq_n_f64(0.0);
        r = vsetq_lane_f64(final_hi, r, 1);
        return vsetq_lane_f64(final_lo, r, 0);
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
#endif

#endif // SIMD_F128_IMPLEMENTATION
#endif // SIMD_F128_H
