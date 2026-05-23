// updated 2026-05-23

#ifndef SIMD_F128_ARRAY_H
#define SIMD_F128_ARRAY_H

#include "simd_f128.h"
#include <stddef.h>


//  █████  ██████  ██████   █████  ██    ██ 
// ██   ██ ██   ██ ██   ██ ██   ██  ██  ██  
// ███████ ██████  ██████  ███████   ████   
// ██   ██ ██   ██ ██   ██ ██   ██    ██    
// ██   ██ ██   ██ ██   ██ ██   ██    ██    
//
// >>array processing api

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) || defined(_MSC_VER)
#define SIMD_F128_RESTRICT __restrict
#else
#define SIMD_F128_RESTRICT restrict
#endif

// auto-vectorized batch processing (needs -O3)

SIMD_F128_INLINE void simd_f128_array_add(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_add(a[i], b[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_sub(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_sub(a[i], b[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_mul(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_mul(a[i], b[i]);
    }
}

SIMD_F128_INLINE void simd_f128_array_div(const simd_f128* SIMD_F128_RESTRICT a, const simd_f128* SIMD_F128_RESTRICT b, simd_f128* SIMD_F128_RESTRICT out, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out[i] = simd_f128_div(a[i], b[i]);
    }
}

#ifdef __cplusplus
}
#endif

#endif // simd_f128_array_h
