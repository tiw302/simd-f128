// updated 2026-06-06

#ifndef SIMD_F128_COMPLEX_H
#define SIMD_F128_COMPLEX_H

#include "simd_f128.h"
#include "simd_f128_math.h"


//  ██████  ██████  ███    ███ ██████  ██      ███████ ██   ██ 
// ██      ██    ██ ████  ████ ██   ██ ██      ██       ██ ██  
// ██      ██    ██ ██ ████ ██ ██████  ██      █████     ███   
// ██      ██    ██ ██  ██  ██ ██      ██      ██       ██ ██  
//  ██████  ██████  ██      ██ ██      ███████ ███████ ██   ██ 
//
// >>complex numbers api

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    simd_f128 real;
    simd_f128 imag;
} simd_f128_complex;

// add two complex numbers
SIMD_F128_INLINE simd_f128_complex simd_f128_complex_add(simd_f128_complex a, simd_f128_complex b) {
    simd_f128_complex res;
    res.real = simd_f128_add(a.real, b.real);
    res.imag = simd_f128_add(a.imag, b.imag);
    return res;
}

// subtract two complex numbers
SIMD_F128_INLINE simd_f128_complex simd_f128_complex_sub(simd_f128_complex a, simd_f128_complex b) {
    simd_f128_complex res;
    res.real = simd_f128_sub(a.real, b.real);
    res.imag = simd_f128_sub(a.imag, b.imag);
    return res;
}

// multiply two complex numbers using the formula:
// (r1 + i1*i) * (r2 + i2*i) = (r1*r2 - i1*i2) + (r1*i2 + r2*i1)*i
SIMD_F128_INLINE simd_f128_complex simd_f128_complex_mul(simd_f128_complex a, simd_f128_complex b) {
    simd_f128_complex res;
    simd_f128 r1r2 = simd_f128_mul(a.real, b.real);
    simd_f128 i1i2 = simd_f128_mul(a.imag, b.imag);
    simd_f128 r1i2 = simd_f128_mul(a.real, b.imag);
    simd_f128 r2i1 = simd_f128_mul(a.imag, b.real);
    
    res.real = simd_f128_sub(r1r2, i1i2);
    res.imag = simd_f128_add(r1i2, r2i1);
    return res;
}

// divide two complex numbers: (a * conj(b)) / |b|^2
SIMD_F128_INLINE simd_f128_complex simd_f128_complex_div(simd_f128_complex a, simd_f128_complex b) {
    simd_f128 d = simd_f128_add(simd_f128_mul(b.real, b.real), simd_f128_mul(b.imag, b.imag));
    simd_f128_complex res;
    res.real = simd_f128_div(simd_f128_add(simd_f128_mul(a.real, b.real), simd_f128_mul(a.imag, b.imag)), d);
    res.imag = simd_f128_div(simd_f128_sub(simd_f128_mul(a.imag, b.real), simd_f128_mul(a.real, b.imag)), d);
    return res;
}

// calculate absolute square magnitude: |z|^2 = real^2 + imag^2
// optimized for fractal rendering escape time checks (saves a square root)
SIMD_F128_INLINE simd_f128 simd_f128_complex_abs_sqr(simd_f128_complex a) {
    return simd_f128_add(simd_f128_mul(a.real, a.real), simd_f128_mul(a.imag, a.imag));
}

// calculate the magnitude (absolute value) of a complex number
SIMD_F128_INLINE simd_f128 simd_f128_complex_abs(simd_f128_complex a) {
    return simd_f128_sqrt(simd_f128_complex_abs_sqr(a));
}

// calculate the phase angle (argument) of a complex number
SIMD_F128_INLINE simd_f128 simd_f128_complex_arg(simd_f128_complex a) {
    return simd_f128_atan2(a.imag, a.real);
}

// calculate the complex conjugate
SIMD_F128_INLINE simd_f128_complex simd_f128_complex_conj(simd_f128_complex a) {
    simd_f128_complex res;
    res.real = a.real;
    res.imag = simd_f128_neg(a.imag);
    return res;
}

#ifdef __cplusplus
}
#endif

#endif // SIMD_F128_COMPLEX_H
