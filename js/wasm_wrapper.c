#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * we use an array of 2 doubles [hi, lo] to communicate seamlessly 
 * with javascript's float64array over wasm memory.
 */

void simd_f128_wasm_add(const double* a, const double* b, double* out) {
    simd_f128 va = {a[0], a[1]};
    simd_f128 vb = {b[0], b[1]};
    simd_f128 res = simd_f128_add(va, vb);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_mul(const double* a, const double* b, double* out) {
    simd_f128 va = {a[0], a[1]};
    simd_f128 vb = {b[0], b[1]};
    simd_f128 res = simd_f128_mul(va, vb);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_sub(const double* a, const double* b, double* out) {
    simd_f128 va = {a[0], a[1]};
    simd_f128 vb = {b[0], b[1]};
    simd_f128 res = simd_f128_sub(va, vb);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_div(const double* a, const double* b, double* out) {
    simd_f128 va = {a[0], a[1]};
    simd_f128 vb = {b[0], b[1]};
    simd_f128 res = simd_f128_div(va, vb);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_from_string(const char* str, double* out) {
    simd_f128 res = simd_f128_from_string(str);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_to_string(const double* a, char* buf, int buf_size) {
    simd_f128 va = {a[0], a[1]};
    simd_f128_to_string(buf, buf_size, va);
}

#ifdef __cplusplus
}
#endif
