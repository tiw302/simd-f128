// updated 2026-05-23

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

// exponential function (e^x)
SIMD_F128_INLINE simd_f128 simd_f128_exp(simd_f128 x);

// natural logarithm (ln x)
SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x);

// power function (base^exp)
SIMD_F128_INLINE simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp);

// trigonometric functions
SIMD_F128_INLINE simd_f128 simd_f128_sin(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x);

// inverse trigonometric functions
SIMD_F128_INLINE simd_f128 simd_f128_atan(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_atan2(simd_f128 y, simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_asin(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_acos(simd_f128 x);

// rounding and remainder
SIMD_F128_INLINE simd_f128 simd_f128_floor(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_ceil(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_trunc(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_round(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_fmod(simd_f128 a, simd_f128 b);

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

    // catch overflow/underflow early
    if (hi > 709.0) return simd_f128_from_double(INFINITY);
    if (hi < -745.0) return simd_f128_from_double(0.0);

    // range reduction (x = k*ln(2) + r) to speed up taylor series
    // 1.4426950408889634 is 1/ln(2)
    double k_double = round(hi * 1.4426950408889634);
    int k = (int)k_double;

    simd_f128 k_f128 = simd_f128_from_double(k_double);
    // r = x - k * ln(2)
    simd_f128 r = simd_f128_sub(x, simd_f128_mul(k_f128, SIMD_F128_LN2));

    // taylor series for e^r: 1 + r + r^2/2! + r^3/3! + ...
    // 23 terms is needed for 128-bit precision limits
    simd_f128 s = simd_f128_from_double(1.0);
    simd_f128 term = simd_f128_from_double(1.0);
    for (int i = 1; i <= 23; i++) {
        // compute next term: term_prev * (r / i)
        term = simd_f128_div(simd_f128_mul(term, r), simd_f128_from_double((double)i));
        s = simd_f128_add(s, term);
    }

    // scale by 2^k (use ldexp to keep relative precision intact without multi-step math)
    double res_hi, res_lo;
    simd_f128_extract(s, &res_hi, &res_lo);
    res_hi = ldexp(res_hi, k);
    res_lo = ldexp(res_lo, k);

    return simd_f128_from_hi_lo(res_hi, res_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    if (hi <= 0.0) return simd_f128_from_double(NAN);

    // halley's method (newton on steroids, converges in 3 iters)
    // formula: y_{n+1} = y_n + 2 * (x - e^{y_n}) / (x + e^{y_n})
    simd_f128 y = simd_f128_from_double(log(hi)); // hardware precision initial guess
    for (int i = 0; i < 3; i++) {
        simd_f128 ey = simd_f128_exp(y);
        simd_f128 num = simd_f128_sub(x, ey);
        simd_f128 den = simd_f128_add(x, ey);
        // delta = 2 * (x - e^y) / (x + e^y)
        simd_f128 delta = simd_f128_mul(simd_f128_from_double(2.0), simd_f128_div(num, den));
        y = simd_f128_add(y, delta);
    }
    return y;
}

SIMD_F128_INLINE simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp) {
    // base^exp = e^(exp * ln(base))
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
     * for production, payne-hanek reduction would be better.
     * 0.15915494309189535 is 1/(2*pi)
     */
    double twopi_inv = 0.15915494309189535;
    double k = round(hi * twopi_inv); // find nearest multiple of 2*pi
    simd_f128 twopi = simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(2.0));
    simd_f128 r = simd_f128_sub(x, simd_f128_mul(simd_f128_from_double(k), twopi)); // r = x - k*2pi

    // taylor series for sin(r): r - r^3/3! + r^5/5! - ...
    // 22 terms is needed for 128-bit precision limits
    simd_f128 s = r;
    simd_f128 term = r;
    simd_f128 rsq = simd_f128_mul(r, r); // precalculate r^2 for stepping
    for (int i = 1; i <= 22; i++) {
        // d is the denominator growth: (2i)*(2i+1)
        double d = (2.0 * i) * (2.0 * i + 1.0);
        // term_next = term_prev * r^2 / (-d)
        term = simd_f128_div(simd_f128_mul(term, rsq), simd_f128_from_double(-d));
        s = simd_f128_add(s, term);
    }
    return s;
}

SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x) {
    // cos(x) = sin(x + pi/2)
    simd_f128 piover2 = simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
    return simd_f128_sin(simd_f128_add(x, piover2));
}

SIMD_F128_INLINE simd_f128 simd_f128_atan(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    // newton-raphson for atan: y_{n+1} = y_n + cos(y_n) * (x * cos(y_n) - sin(y_n))
    simd_f128 y = simd_f128_from_double(atan(hi)); // hardware initial guess
    for (int i = 0; i < 3; i++) {
        simd_f128 sy = simd_f128_sin(y);
        simd_f128 cy = simd_f128_cos(y);
        simd_f128 term = simd_f128_sub(simd_f128_mul(x, cy), sy); // x*cos(y) - sin(y)
        y = simd_f128_add(y, simd_f128_mul(cy, term)); // y + cos(y) * term
    }
    return y;
}

SIMD_F128_INLINE simd_f128 simd_f128_atan2(simd_f128 y, simd_f128 x) {
    double yhi, ylo, xhi, xlo;
    simd_f128_extract(y, &yhi, &ylo);
    simd_f128_extract(x, &xhi, &xlo);
    
    if (xhi == 0.0 && yhi == 0.0) return simd_f128_from_double(0.0);
    
    if (xhi > 0.0) {
        return simd_f128_atan(simd_f128_div(y, x));
    } else if (xhi < 0.0) {
        if (yhi >= 0.0) return simd_f128_add(simd_f128_atan(simd_f128_div(y, x)), SIMD_F128_PI);
        else return simd_f128_sub(simd_f128_atan(simd_f128_div(y, x)), SIMD_F128_PI);
    } else {
        if (yhi > 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
        else return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(-0.5));
    }
}

SIMD_F128_INLINE simd_f128 simd_f128_asin(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    if (hi > 1.0 || hi < -1.0) return simd_f128_from_double(NAN);
    if (hi == 1.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
    if (hi == -1.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(-0.5));
    
    // newton-raphson for asin: y_{n+1} = y_n + (x - sin(y_n)) / cos(y_n)
    simd_f128 y = simd_f128_from_double(asin(hi)); // hardware initial guess
    for (int i = 0; i < 3; i++) {
        simd_f128 sy = simd_f128_sin(y);
        simd_f128 cy = simd_f128_cos(y);
        // correction term = (x - sin(y)) / cos(y)
        y = simd_f128_add(y, simd_f128_div(simd_f128_sub(x, sy), cy));
    }
    return y;
}

SIMD_F128_INLINE simd_f128 simd_f128_acos(simd_f128 x) {
    simd_f128 piover2 = simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
    return simd_f128_sub(piover2, simd_f128_asin(x));
}

SIMD_F128_INLINE simd_f128 simd_f128_floor(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    double fhi = floor(hi);
    double flo = 0.0;
    if (fhi == hi) {
        flo = floor(lo);
    }
    return simd_f128_add(simd_f128_from_double(fhi), simd_f128_from_double(flo));
}

SIMD_F128_INLINE simd_f128 simd_f128_ceil(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    double chi = ceil(hi);
    double clo = 0.0;
    if (chi == hi) {
        clo = ceil(lo);
    }
    return simd_f128_add(simd_f128_from_double(chi), simd_f128_from_double(clo));
}

SIMD_F128_INLINE simd_f128 simd_f128_trunc(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    double thi = trunc(hi);
    double tlo = 0.0;
    if (thi == hi) {
        tlo = trunc(lo);
    }
    return simd_f128_add(simd_f128_from_double(thi), simd_f128_from_double(tlo));
}

SIMD_F128_INLINE simd_f128 simd_f128_round(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    simd_f128 half = simd_f128_from_double(0.5);
    if (hi < 0.0 || (hi == 0.0 && lo < 0.0)) {
        return simd_f128_ceil(simd_f128_sub(x, half));
    } else {
        return simd_f128_floor(simd_f128_add(x, half));
    }
}

SIMD_F128_INLINE simd_f128 simd_f128_fmod(simd_f128 a, simd_f128 b) {
    simd_f128 quotient = simd_f128_div(a, b);
    simd_f128 t = simd_f128_trunc(quotient);
    return simd_f128_sub(a, simd_f128_mul(t, b));
}

#endif // simd_f128_math_h
