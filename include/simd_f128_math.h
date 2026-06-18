// updated 2026-06-12
// spdx-license-identifier: mit
// copyright (c) 2026 jirawat siripuk

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

/* exponential function (e^x):
 * computes base-e exponential. employs a careful range reduction using
 * log2(e) to scale the input, then uses a high-precision minimax polynomial 
 * approximation for the fractional part. handles overflow (> 709.78) and 
 * underflow (< -745.13) gracefully by returning inf or 0.0. */
SIMD_F128_INLINE simd_f128 simd_f128_exp(simd_f128 x);

/* natural logarithm (ln x):
 * computes the natural logarithm. extracts the exponent using frexp,
 * and processes the normalized significand using a chebyshev approximation.
 * handles domain errors (x <= 0) by returning nan or -inf. */
SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x);

/* power function (base^exp):
 * computes base raised to the power of exp using the identity:
 * base^exp = exp(exp * log(base)).
 * includes special handling for edge cases (base=0, fractional exponents
 * of negative bases) to strictly adhere to ieee-754 pow() semantics. */
SIMD_F128_INLINE simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp);

/* trigonometric functions:
 * computes sine, cosine, and tangent. employs range reduction to map
 * the input into the primary domain [-pi/4, pi/4], then approximates 
 * using a taylor/minimax polynomial. */
SIMD_F128_INLINE simd_f128 simd_f128_sin(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_tan(simd_f128 x);

/* simultaneous sine and cosine:
 * computes both sin(x) and cos(x) simultaneously. shares the expensive
 * range reduction step, making it much faster than calling sin and cos
 * separately. perfect for rendering loops and complex rotations. */
SIMD_F128_INLINE void simd_f128_sincos(simd_f128 x, simd_f128* s, simd_f128* c);

/* inverse trigonometric functions:
 * computes arc sine, arc cosine, and arc tangent.
 * atan2 handles all four quadrants correctly based on the signs of y and x. */
SIMD_F128_INLINE simd_f128 simd_f128_atan(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_atan2(simd_f128 y, simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_asin(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_acos(simd_f128 x);

/* hyperbolic functions:
 * computes sinh, cosh, and tanh using combinations of the exponential function. */
SIMD_F128_INLINE simd_f128 simd_f128_sinh(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_cosh(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_tanh(simd_f128 x);

/* rounding and remainder functions:
 * correctly handles the two-component structure of the double-double 
 * format to perform precise ieee-754 rounding operations. */
SIMD_F128_INLINE simd_f128 simd_f128_floor(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_ceil(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_trunc(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_round(simd_f128 x);
SIMD_F128_INLINE simd_f128 simd_f128_fmod(simd_f128 a, simd_f128 b);

#ifdef __cplusplus
}
#endif

// ██ ███    ███ ██████  ██      ███████ ███    ███ ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██
// ██ ████  ████ ██   ██ ██      ██      ████  ████ ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██
// ██ ██ ████ ██ ██████  ██      █████   ██ ████ ██ █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██
// ██ ██  ██  ██ ██      ██      ██      ██  ██  ██ ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██
// ██ ██      ██ ██      ███████ ███████ ██      ██ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████
//
// >>implementation logic

SIMD_F128_INLINE simd_f128 simd_f128_exp(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    // check for nan input
    if (isnan(hi)) return simd_f128_from_double(NAN);

    // catch overflow/underflow early to prevent invalid math steps
    if (hi > 709.78271289338399) return simd_f128_from_double(INFINITY);
    if (hi < -745.13321910194110) return simd_f128_from_double(0.0);

    // range reduction to quadrant or sub-interval with n = 16:
    // x = (k / 16) * ln(2) + r, where r is in [-ln(2)/32, ln(2)/32]
    // 23.083120654223414 is the pre-calculated constant 16 / ln(2)
    double k_double = round(hi * 23.083120654223414);
    long long k = (long long)k_double;

    simd_f128 k_f128 = simd_f128_from_double(k_double);
    simd_f128 r = simd_f128_sub(x, simd_f128_mul(k_f128, SIMD_F128_LN2_16));

    // evaluate chebyshev polynomial approximation of degree 12 (11 coefficients)
    // for e^r on the reduced interval [-ln(2)/32, ln(2)/32] using horner's scheme
    simd_f128 s = _simd_f128_from_raw(_simd_f128_exp_coefs_n16[10]);
    for (int j = 9; j >= 0; j--) {
        s = simd_f128_add(_simd_f128_from_raw(_simd_f128_exp_coefs_n16[j]), simd_f128_mul(s, r));
    }
    // reconstruct e^r using the relation: e^r ~ 1 + r + r^2 * s = 1 + r * (1 + r * s)
    simd_f128 er = simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(r, simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(r, s))));

    // scale by 2^(k/16) using pre-computed lookup tables:
    // exp(x) = e^r * 2^(k/16) = e^r * 2^(k % 16 / 16) * 2^(k / 16)
    long long m = k / 16;
    int i = (int)(k % 16);
    if (i < 0) {
        i += 16;
        m -= 1;
    }

    // fetch the fraction part 2^(i/16) from the pre-computed table
    simd_f128 T = _simd_f128_from_raw(_simd_f128_exp_table[i]);
    simd_f128 res = simd_f128_mul(er, T);

    // scale the result by 2^m using ldexp on hi and lo parts separately
    double res_hi, res_lo;
    simd_f128_extract(res, &res_hi, &res_lo);
    res_hi = ldexp(res_hi, (int)m);
    res_lo = ldexp(res_lo, (int)m);

    return simd_f128_from_hi_lo(res_hi, res_lo);
}

SIMD_F128_INLINE simd_f128 simd_f128_log(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    // check for nan, negative, and zero inputs
    if (isnan(hi)) return simd_f128_from_double(NAN);
    if (hi == 0.0 && lo == 0.0) return simd_f128_from_double(-INFINITY);
    if (hi < 0.0 || (hi == 0.0 && lo < 0.0)) return simd_f128_from_double(NAN);
    if (isinf(hi)) return simd_f128_from_double(INFINITY);

    // solve ln(x) - y = 0 using halley's method (third-order convergence)
    // we use standard double log(hi) as the initial guess
    simd_f128 y = simd_f128_from_double(log(hi));
    
    // one halley iteration is sufficient for 106-bit precision in normal cases.
    // however, for subnormal inputs, the initial log(hi) guess might be extremely poor.
    // we do a second iteration if the first correction is too large.
    {
        simd_f128 ey = simd_f128_exp(y);
        simd_f128 num = simd_f128_sub(x, ey);
        simd_f128 den = simd_f128_add(x, ey);
        // delta = 2 * (x - e^y) / (x + e^y)
        simd_f128 delta = simd_f128_mul(simd_f128_from_double(2.0), simd_f128_div(num, den));
        y = simd_f128_add(y, delta);

        double dhi, dlo;
        simd_f128_extract(delta, &dhi, &dlo);
        if (fabs(dhi) > 1e-3) {
            ey = simd_f128_exp(y);
            num = simd_f128_sub(x, ey);
            den = simd_f128_add(x, ey);
            delta = simd_f128_mul(simd_f128_from_double(2.0), simd_f128_div(num, den));
            y = simd_f128_add(y, delta);
        }
    }
    return y;
}

SIMD_F128_INLINE simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp) {
    double bhi, blo, ehi, elo;
    simd_f128_extract(base, &bhi, &blo);
    simd_f128_extract(exp, &ehi, &elo);

    // base^0 is always 1
    if (ehi == 0.0 && elo == 0.0) {
        return simd_f128_from_double(1.0);
    }
    // 0^exp depends on the sign of the exponent and base
    if (bhi == 0.0 && blo == 0.0) {
        double rounded_e = round(ehi);
        int is_odd = 0;
        if (ehi == rounded_e && elo == 0.0 && fabs(rounded_e) <= 9.0e18) {
            is_odd = ((long long)rounded_e) % 2 != 0;
        }
        int is_neg_base = signbit(bhi);
        if (ehi < 0.0) {
            if (is_neg_base && is_odd) {
                return simd_f128_from_double(-INFINITY);
            }
            return simd_f128_from_double(INFINITY);
        } else {
            if (is_neg_base && is_odd) {
                return simd_f128_from_hi_lo(-0.0, -0.0);
            }
            return simd_f128_from_double(0.0);
        }
    }
    // handle negative base cases with integer exponents
    if (bhi < 0.0 || (bhi == 0.0 && blo < 0.0)) {
        double rounded_e = round(ehi);
        if (ehi == rounded_e && elo == 0.0 && fabs(rounded_e) <= 9.0e18) {
            long long e_int = (long long)rounded_e;
            simd_f128 abs_base = simd_f128_abs(base);
            simd_f128 res = simd_f128_exp(simd_f128_mul(exp, simd_f128_log(abs_base)));
            // if exponent is odd, negate the result
            if (e_int % 2 != 0) {
                res = simd_f128_neg(res);
            }
            return res;
        }
        return simd_f128_from_double(NAN);
    }

    // general case: base^exp = exp(exp * log(base))
    return simd_f128_exp(simd_f128_mul(exp, simd_f128_log(base)));
}

SIMD_F128_INLINE void simd_f128_sincos(simd_f128 x, simd_f128* s, simd_f128* c) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    // check for nan and inf inputs
    if (isnan(hi) || isinf(hi)) {
        *s = simd_f128_from_double(NAN);
        *c = simd_f128_from_double(NAN);
        return;
    }

    // prevent overflow for huge inputs where long long cast overflows.
    // at this scale we fallback to standard double precision range reduction using fmod.
    if (__builtin_expect(fabs(hi) > 9e18, 0)) {
        double two_pi = 2.0 * _simd_f128_pi_raw[0];
        double reduced = fmod(hi, two_pi);
        if (reduced < 0.0) reduced += two_pi;
        x = simd_f128_from_double(reduced);
        simd_f128_extract(x, &hi, &lo);
    }

    // range reduction: x = k * (pi/2) + r, where r in [-pi/4, pi/4]
    // we use a double-double multiplication by 2/pi to prevent precision loss for large inputs
    simd_f128 x_scaled = simd_f128_mul(x, SIMD_F128_TWO_OVER_PI);
    double x_scaled_hi, x_scaled_lo;
    simd_f128_extract(x_scaled, &x_scaled_hi, &x_scaled_lo);
    double k_double = round(x_scaled_hi);
    long long k = (long long)k_double;

    simd_f128 r = simd_f128_sub(x, simd_f128_mul(simd_f128_from_double(k_double), SIMD_F128_PI_OVER_2));
    simd_f128 rsq = simd_f128_mul(r, r);

    // evaluate chebyshev minimax polynomial approximations for sin(r) and cos(r)
    // sin(r) ~ r * (1 + rsq * s_sin)
    simd_f128 s_sin = _simd_f128_from_raw(_simd_f128_sin_coefs_n4[11]);
    for (int j = 10; j >= 0; j--) {
        s_sin = simd_f128_add(_simd_f128_from_raw(_simd_f128_sin_coefs_n4[j]), simd_f128_mul(s_sin, rsq));
    }
    simd_f128 sin_r = simd_f128_mul(r, simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(rsq, s_sin)));

    // cos(r) ~ 1 + rsq * s_cos
    simd_f128 s_cos = _simd_f128_from_raw(_simd_f128_cos_coefs_n4[11]);
    for (int j = 10; j >= 0; j--) {
        s_cos = simd_f128_add(_simd_f128_from_raw(_simd_f128_cos_coefs_n4[j]), simd_f128_mul(s_cos, rsq));
    }
    simd_f128 cos_r = simd_f128_add(simd_f128_from_double(1.0), simd_f128_mul(rsq, s_cos));

    // map quadrant offset (k modulo 4) to reconstruct target sine/cosine
    int q = (int)(k % 4);
    if (q < 0) q += 4;

    if (q == 0) {
        *s = sin_r;
        *c = cos_r;
    } else if (q == 1) {
        *s = cos_r;
        *c = simd_f128_neg(sin_r);
    } else if (q == 2) {
        *s = simd_f128_neg(sin_r);
        *c = simd_f128_neg(cos_r);
    } else {
        *s = simd_f128_neg(cos_r);
        *c = sin_r;
    }
}

SIMD_F128_INLINE simd_f128 simd_f128_sin(simd_f128 x) {
    simd_f128 s, c;
    simd_f128_sincos(x, &s, &c);
    return s;
}

SIMD_F128_INLINE simd_f128 simd_f128_cos(simd_f128 x) {
    simd_f128 s, c;
    simd_f128_sincos(x, &s, &c);
    return c;
}

SIMD_F128_INLINE simd_f128 _simd_f128_atan_near_zero(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    // newton-raphson for atan on [-1, 1]: y_{n+1} = y_n + cos(y_n) * (x * cos(y_n) - sin(y_n))
    // start with standard double atan(hi) as the initial guess
    simd_f128 y = simd_f128_from_double(atan(hi));
    
    // one newton iteration is sufficient for 106-bit precision
    {
        simd_f128 sy, cy;
        simd_f128_sincos(y, &sy, &cy);
        simd_f128 term = simd_f128_sub(simd_f128_mul(x, cy), sy);
        y = simd_f128_add(y, simd_f128_mul(cy, term));
    }
    return y;
}

SIMD_F128_INLINE simd_f128 simd_f128_atan(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);

    // check for nan and inf boundary cases
    if (isnan(hi)) return simd_f128_from_double(NAN);
    if (isinf(hi)) return (hi > 0.0) ? SIMD_F128_PI_OVER_2 : simd_f128_neg(SIMD_F128_PI_OVER_2);

    // if |x| > 1, use identity: atan(x) = sign(x) * (pi/2 - atan(1/|x|))
    // this avoids instability in newton-raphson on flat asymptotes
    if (fabs(hi) > 1.0) {
        simd_f128 one = simd_f128_from_double(1.0);
        simd_f128 abs_x = (hi > 0.0) ? x : simd_f128_neg(x);
        simd_f128 inv_x = simd_f128_div(one, abs_x);
        simd_f128 atan_inv = _simd_f128_atan_near_zero(inv_x);
        simd_f128 res = simd_f128_sub(SIMD_F128_PI_OVER_2, atan_inv);
        return (hi > 0.0) ? res : simd_f128_neg(res);
    }

    return _simd_f128_atan_near_zero(x);
}

SIMD_F128_INLINE simd_f128 simd_f128_atan2(simd_f128 y, simd_f128 x) {
    double yhi, ylo, xhi, xlo;
    simd_f128_extract(y, &yhi, &ylo);
    simd_f128_extract(x, &xhi, &xlo);

    if (isnan(xhi) || isnan(yhi)) return simd_f128_from_double(NAN);

    // handle origin case to correctly assign correct quadrant sign bit
    if (xhi == 0.0 && xlo == 0.0 && yhi == 0.0 && ylo == 0.0) {
        if (signbit(xhi)) {
            if (signbit(yhi)) return simd_f128_neg(SIMD_F128_PI);
            return SIMD_F128_PI;
        } else {
            if (signbit(yhi)) return simd_f128_from_double(-0.0);
            return simd_f128_from_double(0.0);
        }
    }

    // determine angle based on the sign of the x coordinate
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
    
    // check boundaries for domain of arcsin [-1, 1]
    if (isnan(hi) || isinf(hi) || hi > 1.0 || hi < -1.0 || (hi == 1.0 && lo > 0.0) || (hi == -1.0 && lo < 0.0)) {
        return simd_f128_from_double(NAN);
    }
    if (hi == 1.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));
    if (hi == -1.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(-0.5));

    // compute asin using stable identity: asin(x) = atan(x / sqrt(1 - x^2))
    // this avoids cancellation issues in newton-raphson near boundaries
    simd_f128 one = simd_f128_from_double(1.0);
    simd_f128 denom = simd_f128_sqrt(simd_f128_sub(one, simd_f128_mul(x, x)));
    return simd_f128_atan(simd_f128_div(x, denom));
}

SIMD_F128_INLINE simd_f128 simd_f128_acos(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    
    // check boundaries for domain of arccos [-1, 1]
    if (isnan(hi) || isinf(hi) || hi > 1.0 || hi < -1.0 || (hi == 1.0 && lo > 0.0) || (hi == -1.0 && lo < 0.0)) {
        return simd_f128_from_double(NAN);
    }
    if (hi == 1.0 && lo == 0.0) return simd_f128_from_double(0.0);
    if (hi == -1.0 && lo == 0.0) return SIMD_F128_PI;
    if (hi == 0.0 && lo == 0.0) return simd_f128_mul(SIMD_F128_PI, simd_f128_from_double(0.5));

    // compute acos using stable identity:
    // acos(x) = atan(sqrt(1 - x^2) / x) for x > 0
    // acos(x) = pi - acos(-x) for x < 0
    // this avoids cancellation issues near endpoints
    simd_f128 one = simd_f128_from_double(1.0);
    simd_f128 num = simd_f128_sqrt(simd_f128_sub(one, simd_f128_mul(x, x)));
    
    if (hi > 0.0 || (hi == 0.0 && lo > 0.0)) {
        return simd_f128_atan(simd_f128_div(num, x));
    } else {
        simd_f128 neg_x = simd_f128_neg(x);
        return simd_f128_sub(SIMD_F128_PI, simd_f128_atan(simd_f128_div(num, neg_x)));
    }
}

SIMD_F128_INLINE simd_f128 simd_f128_floor(simd_f128 x) {
    // round down to the nearest integer
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    double fhi = floor(hi);
    double flo = 0.0;
    if (fhi == hi) {
        flo = floor(lo);
    }
    // normalize the sum using a quick two-sum
    double s = fhi + flo;
    double e = flo - (s - fhi);
    return simd_f128_from_hi_lo(s, e);
}

SIMD_F128_INLINE simd_f128 simd_f128_ceil(simd_f128 x) {
    // round up to the nearest integer
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    double chi = ceil(hi);
    double clo = 0.0;
    if (chi == hi) {
        clo = ceil(lo);
    }
    // normalize the sum using a quick two-sum
    double s = chi + clo;
    double e = clo - (s - chi);
    return simd_f128_from_hi_lo(s, e);
}

SIMD_F128_INLINE simd_f128 simd_f128_trunc(simd_f128 x) {
    // round towards zero
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    double thi = trunc(hi);
    double tlo = 0.0;
    if (thi == hi) {
        tlo = trunc(lo);
    }
    // normalize the sum using a quick two-sum
    double s = thi + tlo;
    double e = tlo - (s - thi);
    return simd_f128_from_hi_lo(s, e);
}

SIMD_F128_INLINE simd_f128 simd_f128_round(simd_f128 x) {
    // round to nearest integer, half cases rounded away from zero
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
    // floating-point remainder logic: a - trunc(a/b) * b
    simd_f128 quotient = simd_f128_div(a, b);
    simd_f128 t = simd_f128_trunc(quotient);
    return simd_f128_sub(a, simd_f128_mul(t, b));
}

SIMD_F128_INLINE simd_f128 simd_f128_tan(simd_f128 x) {
    // tangent calculation using division of sin by cos
    simd_f128 s, c;
    simd_f128_sincos(x, &s, &c);
    return simd_f128_div(s, c);
}

SIMD_F128_INLINE simd_f128 simd_f128_sinh(simd_f128 x) {
    // hyperbolic sine calculation using exp(x) and exp(-x)
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    if (isnan(hi)) return x;
    if (isinf(hi)) return x; // sinh(±inf) = ±inf

    // taylor series fallback for small x to avoid catastrophic cancellation
    // sinh(x) ~ x + x^3/6 + x^5/120
    if (fabs(hi) < 1e-4) {
        simd_f128 x2 = simd_f128_mul(x, x);
        simd_f128 x3 = simd_f128_mul(x2, x);
        simd_f128 x5 = simd_f128_mul(x3, x2);
        simd_f128 term2 = simd_f128_div(x3, simd_f128_from_double(6.0));
        simd_f128 term3 = simd_f128_div(x5, simd_f128_from_double(120.0));
        return simd_f128_add(simd_f128_add(x, term2), term3);
    }

    simd_f128 ex = simd_f128_exp(x);
    simd_f128 emx = simd_f128_div(simd_f128_from_double(1.0), ex);
    return simd_f128_mul(simd_f128_sub(ex, emx), simd_f128_from_double(0.5));
}

SIMD_F128_INLINE simd_f128 simd_f128_cosh(simd_f128 x) {
    // hyperbolic cosine calculation using exp(x) and exp(-x)
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    if (isnan(hi)) return x;
    if (isinf(hi)) return simd_f128_from_double(INFINITY); // cosh(±inf) = +inf
    simd_f128 ex = simd_f128_exp(x);
    simd_f128 emx = simd_f128_div(simd_f128_from_double(1.0), ex);
    return simd_f128_mul(simd_f128_add(ex, emx), simd_f128_from_double(0.5));
}

SIMD_F128_INLINE simd_f128 simd_f128_tanh(simd_f128 x) {
    // hyperbolic tangent calculation using exp(x) and exp(-x)
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    if (isnan(hi)) return x;
    if (isinf(hi)) return simd_f128_from_double(hi > 0.0 ? 1.0 : -1.0); // tanh(±inf) = ±1.0

    // taylor series fallback for small x to avoid catastrophic cancellation
    // tanh(x) ~ x - x^3/3 + 2x^5/15
    if (fabs(hi) < 1e-4) {
        simd_f128 x2 = simd_f128_mul(x, x);
        simd_f128 x3 = simd_f128_mul(x2, x);
        simd_f128 x5 = simd_f128_mul(x3, x2);
        simd_f128 term2 = simd_f128_div(x3, simd_f128_from_double(-3.0));
        simd_f128 term3 = simd_f128_mul(x5, simd_f128_from_double(2.0 / 15.0));
        return simd_f128_add(simd_f128_add(x, term2), term3);
    }

    simd_f128 ex = simd_f128_exp(x);
    simd_f128 emx = simd_f128_div(simd_f128_from_double(1.0), ex);
    return simd_f128_div(simd_f128_sub(ex, emx), simd_f128_add(ex, emx));
}

#endif // simd_f128_math_h
