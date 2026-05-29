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

    if (isnan(hi)) return simd_f128_from_double(NAN);

    // catch overflow/underflow early
    if (hi > 709.0) return simd_f128_from_double(INFINITY);
    if (hi < -745.0) return simd_f128_from_double(0.0);

    // range reduction to N=16: x = (k / 16) * ln(2) + r
    // 23.083120654223414 is 16 / ln(2)
    double k_double = round(hi * 23.083120654223414);
    long long k = (long long)k_double;

    simd_f128 k_f128 = simd_f128_from_double(k_double);
    simd_f128 r = simd_f128_sub(x, simd_f128_mul(k_f128, SIMD_F128_LN2_16));

    // chebyshev polynomial approximation of degree 12 (k=10) for e^r on [-ln2/32, ln2/32]
    simd_f128 s = _simd_f128_from_raw(_simd_f128_exp_coefs_n16[10]);
    for (int j = 9; j >= 0; j--) {
        s = simd_f128_add(_simd_f128_from_raw(_simd_f128_exp_coefs_n16[j]), simd_f128_mul(s, r));
    }
    // e^r ~ 1 + r + r^2 * s = 1 + r * (1 + r * s)
    simd_f128 er = simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(r, simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(r, s))));

    // exp(x) = e^r * 2^(k/16) = e^r * 2^(k % 16 / 16) * 2^(k / 16)
    long long m = k / 16;
    int i = (int)(k % 16);
    if (i < 0) {
        i += 16;
        m -= 1;
    }

    simd_f128 T = _simd_f128_from_raw(_simd_f128_exp_table[i]);
    simd_f128 res = simd_f128_mul(er, T);

    double res_hi, res_lo;
    simd_f128_extract(res, &res_hi, &res_lo);
    res_hi = ldexp(res_hi, (int)m);
    res_lo = ldexp(res_lo, (int)m);

    return simd_f128_from_hi_lo(res_hi, res_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    if (isnan(hi)) return simd_f128_from_double(NAN);
    if (isinf(hi)) return simd_f128_from_double(INFINITY);
    if (hi < 0.0) return simd_f128_from_double(NAN);
    if (hi == 0.0 && lo <= 0.0) return simd_f128_from_double(-INFINITY);

    // halley's method (newton on steroids, converges in 3 iters)
    // formula: y_{n+1} = y_n + 2 * (x - e^{y_n}) / (x + e^{y_n})
    simd_f128 y = simd_f128_from_double(log(hi)); // hardware precision initial guess
    // 2 iterations of Halley's method is mathematically sufficient for 106-bit precision
    for (int i = 0; i < 2; i++) {
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

    if (isnan(hi) || isinf(hi)) return simd_f128_from_double(NAN);

    // range reduction to quadrant: x = k * (pi/2) + r, where r in [-pi/4, pi/4]
    // 0.6366197723675814 is 2/pi
    double k_double = round(hi * 0.6366197723675814);
    long long k = (long long)k_double;

    simd_f128 r = simd_f128_sub(x, simd_f128_mul(simd_f128_from_double(k_double), SIMD_F128_PI_OVER_2));
    simd_f128 rsq = simd_f128_mul(r, r);

    // evaluate Chebyshev approximations for sin(r) and cos(r) on [-pi/4, pi/4]
    // sin(r) = r * (1 + rsq * s_sin)
    simd_f128 s_sin = _simd_f128_from_raw(_simd_f128_sin_coefs_n4[11]);
    for (int j = 10; j >= 0; j--) {
        s_sin = simd_f128_add(_simd_f128_from_raw(_simd_f128_sin_coefs_n4[j]), simd_f128_mul(s_sin, rsq));
    }
    simd_f128 sin_r = simd_f128_mul(r, simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(rsq, s_sin)));

    // cos(r) = 1 + rsq * s_cos
    simd_f128 s_cos = _simd_f128_from_raw(_simd_f128_cos_coefs_n4[11]);
    for (int j = 10; j >= 0; j--) {
        s_cos = simd_f128_add(_simd_f128_from_raw(_simd_f128_cos_coefs_n4[j]), simd_f128_mul(s_cos, rsq));
    }
    simd_f128 cos_r = simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(rsq, s_cos));

    int q = (int)(k % 4);
    if (q < 0) q += 4;

    if (q == 0) return sin_r;
    if (q == 1) return cos_r;
    if (q == 2) return simd_f128_neg(sin_r);
    return simd_f128_neg(cos_r);
}

SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    if (isnan(hi) || isinf(hi)) return simd_f128_from_double(NAN);

    // range reduction to quadrant: x = k * (pi/2) + r
    double k_double = round(hi * 0.6366197723675814);
    long long k = (long long)k_double;

    simd_f128 r = simd_f128_sub(x, simd_f128_mul(simd_f128_from_double(k_double), SIMD_F128_PI_OVER_2));
    simd_f128 rsq = simd_f128_mul(r, r);

    // evaluate Chebyshev approximations for sin(r) and cos(r)
    simd_f128 s_sin = _simd_f128_from_raw(_simd_f128_sin_coefs_n4[11]);
    for (int j = 10; j >= 0; j--) {
        s_sin = simd_f128_add(_simd_f128_from_raw(_simd_f128_sin_coefs_n4[j]), simd_f128_mul(s_sin, rsq));
    }
    simd_f128 sin_r = simd_f128_mul(r, simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(rsq, s_sin)));

    simd_f128 s_cos = _simd_f128_from_raw(_simd_f128_cos_coefs_n4[11]);
    for (int j = 10; j >= 0; j--) {
        s_cos = simd_f128_add(_simd_f128_from_raw(_simd_f128_cos_coefs_n4[j]), simd_f128_mul(s_cos, rsq));
    }
    simd_f128 cos_r = simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(rsq, s_cos));

    int q = (int)(k % 4);
    if (q < 0) q += 4;

    if (q == 0) return cos_r;
    if (q == 1) return simd_f128_neg(sin_r);
    if (q == 2) return simd_f128_neg(cos_r);
    return sin_r;
}

SIMD_F128_INLINE simd_f128 simd_f128_atan(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    if (isnan(hi)) return simd_f128_from_double(NAN);
    if (isinf(hi)) return (hi > 0.0) ? SIMD_F128_PI_OVER_2 : simd_f128_neg(SIMD_F128_PI_OVER_2);

    // newton-raphson for atan: y_{n+1} = y_n + cos(y_n) * (x * cos(y_n) - sin(y_n))
    simd_f128 y = simd_f128_from_double(atan(hi)); // hardware initial guess
    // 2 iterations of Newton-Raphson is mathematically sufficient for 106-bit precision
    for (int i = 0; i < 2; i++) {
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
    
    if (isnan(xhi) || isnan(yhi)) return simd_f128_from_double(NAN);
    
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
    if (isnan(hi) || isinf(hi) || hi > 1.0 || hi < -1.0) return simd_f128_from_double(NAN);
    if (hi == 1.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
    if (hi == -1.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(-0.5));
    
    // newton-raphson for asin: y_{n+1} = y_n + (x - sin(y_n)) / cos(y_n)
    simd_f128 y = simd_f128_from_double(asin(hi)); // hardware initial guess
    // 2 iterations of Newton-Raphson is mathematically sufficient for 106-bit precision
    for (int i = 0; i < 2; i++) {
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
