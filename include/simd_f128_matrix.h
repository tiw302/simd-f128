/* simd_f128_matrix.h
 *
 * high-precision matrix operations for 3d graphics and physics simulations.
 * provides 2x2, 3x3, and 4x4 double-double matrices to prevent precision loss.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#ifndef SIMD_F128_MATRIX_H
#define SIMD_F128_MATRIX_H

#include "simd_f128.h"
#include "simd_f128_math.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ███    ███  █████  ████████ ██████  ██ ██   ██
// ████  ████ ██   ██    ██    ██   ██ ██  ██ ██
// ██ ████ ██ ███████    ██    ██████  ██   ███
// ██  ██  ██ ██   ██    ██    ██   ██ ██  ██ ██
// ██      ██ ██   ██    ██    ██   ██ ██ ██   ██
//
// >>matrix operations api
typedef struct { simd_f128 m[2][2]; } simd_f128_mat2;
typedef struct { simd_f128 m[3][3]; } simd_f128_mat3;
typedef struct { simd_f128 m[4][4]; } simd_f128_mat4;

typedef struct { simd_f128 x, y; } simd_f128_vec2;
typedef struct { simd_f128 x, y, z; } simd_f128_vec3;
typedef struct { simd_f128 x, y, z, w; } simd_f128_vec4;

SIMD_F128_INLINE simd_f128_mat4 simd_f128_mat4_identity(void) {
    simd_f128_mat4 r;
    simd_f128 zero = simd_f128_from_double(0.0);
    simd_f128 one = simd_f128_from_double(1.0);
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            r.m[i][j] = (i == j) ? one : zero;
        }
    }
    return r;
}

SIMD_F128_INLINE simd_f128_mat4 simd_f128_mat4_mul(simd_f128_mat4 a, simd_f128_mat4 b) {
    simd_f128_mat4 r;
    simd_f128 zero = simd_f128_from_double(0.0);
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            r.m[i][j] = zero;
            for (int k=0; k<4; k++) {
                r.m[i][j] = simd_f128_add(r.m[i][j], simd_f128_mul(a.m[i][k], b.m[k][j]));
            }
        }
    }
    return r;
}

SIMD_F128_INLINE simd_f128_vec4 simd_f128_mat4_mul_vec4(simd_f128_mat4 a, simd_f128_vec4 v) {
    simd_f128_vec4 r;
    simd_f128 vec[4] = {v.x, v.y, v.z, v.w};
    simd_f128 out[4];
    simd_f128 zero = simd_f128_from_double(0.0);
    for (int i=0; i<4; i++) {
        out[i] = zero;
        for (int k=0; k<4; k++) {
            out[i] = simd_f128_add(out[i], simd_f128_mul(a.m[i][k], vec[k]));
        }
    }
    r.x = out[0]; r.y = out[1]; r.z = out[2]; r.w = out[3];
    return r;
}

SIMD_F128_INLINE simd_f128_mat3 simd_f128_mat3_identity(void) {
    simd_f128_mat3 r;
    simd_f128 zero = simd_f128_from_double(0.0);
    simd_f128 one = simd_f128_from_double(1.0);
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            r.m[i][j] = (i == j) ? one : zero;
        }
    }
    return r;
}

SIMD_F128_INLINE simd_f128_mat3 simd_f128_mat3_mul(simd_f128_mat3 a, simd_f128_mat3 b) {
    simd_f128_mat3 r;
    simd_f128 zero = simd_f128_from_double(0.0);
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            r.m[i][j] = zero;
            for (int k=0; k<3; k++) {
                r.m[i][j] = simd_f128_add(r.m[i][j], simd_f128_mul(a.m[i][k], b.m[k][j]));
            }
        }
    }
    return r;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SIMD_F128_MATRIX_H
