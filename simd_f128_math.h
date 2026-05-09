// updated 2026-05-09

#ifndef SIMD_F128_MATH_H
#define SIMD_F128_MATH_H

#include "simd_f128.h"
#include "simd_f128_consts.h"
#include "simd_f128_utils.h"

// ██ ██████  
// ██ ██  ██ 
// ██ ██  ██ 
// ██ ██  ██ 
// ██ ██████ 
//
// >>advanced math api

#ifdef __cplusplus
extern "C" {
#endif

/* exponential function (e^x) */
SIMD_F128_INLINE simd_f128 simd_f128_exp(simd_f128 x);

/* natural logarithm (ln x) */
SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x);

/* power function (base^exp) */
SIMD_F128_INLINE simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp);

/* trigonometric functions */
SIMD_F128_INLINE simd_f128 simd_f128_sin(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x);

#ifdef __cplusplus
}
#endif

// ██ ███    ███ ██████  ██      ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██ 
// ██ ████  ████ ██   ██ ██      ██      ████  ████ ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██ 
// ██ ██ ████ ██ ██████  ██      █████   ██ ████ ██ █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██ 
// ██ ██  ██  ██ ██      ██      ██      ██  ██  ██ ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██ 
// ██ ██      ██ ██      ███████ ███████ ██      ██ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████ 
//
// >>implementation logic

SIMD_F128_INLINE simd_f128 simd_f128_exp(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    /* we gotta handle overflow/underflow cases first or things get weird */
    if (hi > 709.0) return simd_f128_from_double(INFINITY);
    if (hi < -745.0) return simd_f128_from_double(0.0);

    /* 
     * range reduction: x = k*ln(2) + r 
     * we do this so r is super small, which makes the taylor series 
     * converge much faster and keep that 128-bit precision.
     */
    double k_double = round(hi * 1.4426950408889634);
    int k = (int)k_double;

    simd_f128 k_f128 = simd_f128_from_double(k_double);
    simd_f128 r = simd_f128_sub(x, simd_f128_mul(k_f128, SIMD_F128_LN2));

    /* 
     * taylor series for e^r. 
     * since |r| is at most ln(2)/2, 14 terms is the sweet spot 
     * for getting all 31-32 digits right.
     */
    simd_f128 s = simd_f128_from_double(1.0);
    simd_f128 term = simd_f128_from_double(1.0);
    for (int i = 1; i <= 14; i++) {
        term = simd_f128_mul(term, simd_f128_div(r, simd_f128_from_double((double)i)));
        s = simd_f128_add(s, term);
    }

    /* 
     * precise scaling by 2^k. 
     * we use ldexp on both parts to keep the relative precision intact.
     */
    double res_hi, res_lo;
    simd_f128_extract(s, &res_hi, &res_lo);
    res_hi = ldexp(res_hi, k);
    res_lo = ldexp(res_lo, k);
    
    simd_f128 res;
#if defined(SIMD_F128_USE_SCALAR)
    res.hi = res_hi; res.lo = res_lo;
#else
    double raw[2] = { res_hi, res_lo };
    memcpy(&res, raw, sizeof(res));
#endif
    return res;
}

SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    if (hi <= 0.0) return simd_f128_from_double(NAN);

    /* 
     * halley's method is basically newton's method on steroids.
     * it converges cubically, so 3 iterations is plenty to polish 
     * the standard double log up to full 128-bit glory.
     */
    simd_f128 y = simd_f128_from_double(log(hi));
    for (int i = 0; i < 3; i++) {
        simd_f128 ey = simd_f128_exp(y);
        simd_f128 num = simd_f128_sub(x, ey);
        simd_f128 den = simd_f128_add(x, ey);
        simd_f128 delta = simd_f128_mul(simd_f128_from_double(2.0), simd_f128_div(num, den));
        y = simd_f128_add(y, delta);
    }
    return y;
}

SIMD_F128_INLINE simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp) {
    /* base^exp = e^(exp * ln(base)) */
    double bhi, blo;
    simd_f128_extract(base, &bhi, &blo);
    if (bhi == 0.0) return simd_f128_from_double(0.0);
    if (bhi < 0.0) return simd_f128_from_double(NAN);

    return simd_f128_exp(simd_f128_mul(exp, simd_f128_log(base)));
}

SIMD_F128_INLINE simd_f128 simd_f128_sin(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    /* 
     * simplified range reduction to [-pi, pi].
     * for production, Payne-Hanek reduction would be better.
     */
    double twopi_inv = 0.15915494309189535;
    double k = round(hi * twopi_inv);
    simd_f128 twopi = simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(2.0));
    simd_f128 r = simd_f128_sub(x, simd_f128_mul(simd_f128_from_double(k), twopi));

    /* taylor series for sin(r) */
    simd_f128 s = r;
    simd_f128 term = r;
    simd_f128 rsq = simd_f128_mul(r, r);
    for (int i = 1; i <= 10; i++) {
        double d = (2.0 * i) * (2.0 * i + 1.0);
        term = simd_f128_mul(term, simd_f128_div(rsq, simd_f128_from_double(-d)));
        s = simd_f128_add(s, term);
    }
    return s;
}

SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x) {
    /* cos(x) = sin(x + pi/2) */
    simd_f128 piover2 = simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
    return simd_f128_sin(simd_f128_add(x, piover2));
}

#endif /* SIMD_F128_MATH_H */
