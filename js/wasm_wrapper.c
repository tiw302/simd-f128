// updated 2026-06-06

#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"
#include "../include/simd_f128_math.h"
#include "../include/simd_f128_utils.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// no more global state. passing pointer from js to avoid race conditions.

#define WASM_BIN_OP(name, func) \
    void simd_f128_wasm_##name(double a_hi, double a_lo, double b_hi, double b_lo, double* out) { \
        simd_f128 res = func(simd_f128_from_hi_lo(a_hi, a_lo), simd_f128_from_hi_lo(b_hi, b_lo)); \
        simd_f128_extract(res, &out[0], &out[1]); \
    }

#define WASM_UN_OP(name, func) \
    void simd_f128_wasm_##name(double a_hi, double a_lo, double* out) { \
        simd_f128 res = func(simd_f128_from_hi_lo(a_hi, a_lo)); \
        simd_f128_extract(res, &out[0], &out[1]); \
    }

WASM_BIN_OP(add, simd_f128_add)
WASM_BIN_OP(sub, simd_f128_sub)
WASM_BIN_OP(mul, simd_f128_mul)
WASM_BIN_OP(div, simd_f128_div)
WASM_BIN_OP(pow, simd_f128_pow)
WASM_BIN_OP(fmod, simd_f128_fmod)
WASM_BIN_OP(atan2, simd_f128_atan2)

WASM_UN_OP(exp, simd_f128_exp)
WASM_UN_OP(log, simd_f128_log)
WASM_UN_OP(sin, simd_f128_sin)
WASM_UN_OP(cos, simd_f128_cos)
WASM_UN_OP(sqrt, simd_f128_sqrt)
WASM_UN_OP(abs, simd_f128_abs)
WASM_UN_OP(floor, simd_f128_floor)
WASM_UN_OP(ceil, simd_f128_ceil)
WASM_UN_OP(round, simd_f128_round)
WASM_UN_OP(trunc, simd_f128_trunc)
WASM_UN_OP(atan, simd_f128_atan)
WASM_UN_OP(asin, simd_f128_asin)
WASM_UN_OP(acos, simd_f128_acos)
WASM_UN_OP(tan, simd_f128_tan)
WASM_UN_OP(sinh, simd_f128_sinh)
WASM_UN_OP(cosh, simd_f128_cosh)
WASM_UN_OP(tanh, simd_f128_tanh)

void simd_f128_wasm_from_string(const char* str, double* out) {
    simd_f128 res = simd_f128_from_string(str);
    simd_f128_extract(res, &out[0], &out[1]);
}

void simd_f128_wasm_to_string(double a_hi, double a_lo, char* buf, int buf_size) {
    simd_f128_to_string(buf, buf_size, simd_f128_from_hi_lo(a_hi, a_lo));
}

void simd_f128_wasm_to_string_prec(double a_hi, double a_lo, char* buf, int buf_size, int digits) {
    simd_f128_to_string_prec(buf, buf_size, simd_f128_from_hi_lo(a_hi, a_lo), digits);
}

void simd_f128_wasm_sincos(double a_hi, double a_lo, double* out_s, double* out_c) {
    simd_f128 s, c;
    simd_f128_sincos(simd_f128_from_hi_lo(a_hi, a_lo), &s, &c);
    simd_f128_extract(s, &out_s[0], &out_s[1]);
    simd_f128_extract(c, &out_c[0], &out_c[1]);
}

int simd_f128_wasm_isnan(double a_hi, double a_lo) {
    return simd_f128_isnan(simd_f128_from_hi_lo(a_hi, a_lo));
}

int simd_f128_wasm_isinf(double a_hi, double a_lo) {
    return simd_f128_isinf(simd_f128_from_hi_lo(a_hi, a_lo));
}

int simd_f128_wasm_cmp(double a_hi, double a_lo, double b_hi, double b_lo) {
    return simd_f128_cmp(simd_f128_from_hi_lo(a_hi, a_lo), simd_f128_from_hi_lo(b_hi, b_lo));
}

#ifdef __cplusplus
}
#endif
