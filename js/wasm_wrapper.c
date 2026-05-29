#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Global/static variable to store the low double of the last calculation
double wasm_low_result = 0.0;

double simd_f128_wasm_add(double a_hi, double a_lo, double b_hi, double b_lo) {
    simd_f128 va = simd_f128_from_hi_lo(a_hi, a_lo);
    simd_f128 vb = simd_f128_from_hi_lo(b_hi, b_lo);
    simd_f128 res = simd_f128_add(va, vb);
    double r_hi, r_lo;
    simd_f128_extract(res, &r_hi, &r_lo);
    wasm_low_result = r_lo;
    return r_hi;
}

double simd_f128_wasm_sub(double a_hi, double a_lo, double b_hi, double b_lo) {
    simd_f128 va = simd_f128_from_hi_lo(a_hi, a_lo);
    simd_f128 vb = simd_f128_from_hi_lo(b_hi, b_lo);
    simd_f128 res = simd_f128_sub(va, vb);
    double r_hi, r_lo;
    simd_f128_extract(res, &r_hi, &r_lo);
    wasm_low_result = r_lo;
    return r_hi;
}

double simd_f128_wasm_mul(double a_hi, double a_lo, double b_hi, double b_lo) {
    simd_f128 va = simd_f128_from_hi_lo(a_hi, a_lo);
    simd_f128 vb = simd_f128_from_hi_lo(b_hi, b_lo);
    simd_f128 res = simd_f128_mul(va, vb);
    double r_hi, r_lo;
    simd_f128_extract(res, &r_hi, &r_lo);
    wasm_low_result = r_lo;
    return r_hi;
}

double simd_f128_wasm_div(double a_hi, double a_lo, double b_hi, double b_lo) {
    simd_f128 va = simd_f128_from_hi_lo(a_hi, a_lo);
    simd_f128 vb = simd_f128_from_hi_lo(b_hi, b_lo);
    simd_f128 res = simd_f128_div(va, vb);
    double r_hi, r_lo;
    simd_f128_extract(res, &r_hi, &r_lo);
    wasm_low_result = r_lo;
    return r_hi;
}

double simd_f128_wasm_get_low() {
    return wasm_low_result;
}

void simd_f128_wasm_from_string(const char* str, double* out) {
    simd_f128 res = simd_f128_from_string(str);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_to_string(double a_hi, double a_lo, char* buf, int buf_size) {
    simd_f128 va = simd_f128_from_hi_lo(a_hi, a_lo);
    simd_f128_to_string(buf, buf_size, va);
}

#ifdef __cplusplus
}
#endif
