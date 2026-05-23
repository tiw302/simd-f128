// updated 2026-05-23

#ifndef SIMD_F128_COMPLEX_H
#define SIMD_F128_COMPLEX_H

#include "simd_f128.h"


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

SIMD_F128_INLINE simd_f128_complex simd_f128_complex_add(simd_f128_complex a, simd_f128_complex b) {
    simd_f128_complex res;
    res.real = simd_f128_add(a.real, b.real);
    res.imag = simd_f128_add(a.imag, b.imag);
    return res;
}

SIMD_F128_INLINE simd_f128_complex simd_f128_complex_sub(simd_f128_complex a, simd_f128_complex b) {
    simd_f128_complex res;
    res.real = simd_f128_sub(a.real, b.real);
    res.imag = simd_f128_sub(a.imag, b.imag);
    return res;
}

SIMD_F128_INLINE simd_f128_complex simd_f128_complex_mul(simd_f128_complex a, simd_f128_complex b) {
    // (r1+i1*i) * (r2+i2*i)
    simd_f128_complex res;
    simd_f128 r1r2 = simd_f128_mul(a.real, b.real);
    simd_f128 i1i2 = simd_f128_mul(a.imag, b.imag);
    simd_f128 r1i2 = simd_f128_mul(a.real, b.imag);
    simd_f128 r2i1 = simd_f128_mul(a.imag, b.real);
    
    res.real = simd_f128_sub(r1r2, i1i2);
    res.imag = simd_f128_add(r1i2, r2i1);
    return res;
}

// |z|^2 (good for mandelbrot checks)
SIMD_F128_INLINE simd_f128 simd_f128_complex_abs_sqr(simd_f128_complex a) {
    return simd_f128_add(simd_f128_mul(a.real, a.real), simd_f128_mul(a.imag, a.imag));
}

#ifdef __cplusplus
}
#endif

#endif // simd_f128_complex_h
