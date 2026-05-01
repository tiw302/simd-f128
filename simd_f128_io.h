#ifndef SIMD_F128_IO_H
#define SIMD_F128_IO_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "simd_f128.h"

// ██ ██████  
// ██ ██  ██ 
// ██ ██  ██ 
// ██ ██  ██ 
// ██ ██████ 
//
// >>input / output api

#ifdef __cplusplus
extern "C" {
#endif

/* prints the 128-bit number to stdout */
void simd_f128_print(simd_f128 x);

/* converts the number to a string buffer */
void simd_f128_to_string(char* buf, size_t buf_size, simd_f128 x);

#ifdef __cplusplus
}
#endif

#ifdef SIMD_F128_IMPLEMENTATION

// ██ ███    ███ ██████  ██      ███████ ███    ███ ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██ 
// ██ ████  ████ ██   ██ ██      ██      ████  ████ ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██ 
// ██ ██ ████ ██ ██████  ██      █████   ██ ████ ██ █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██ 
// ██ ██  ██  ██ ██      ██      ██      ██  ██  ██ ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██ 
// ██ ██      ██ ██      ███████ ███████ ██      ██ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████ 
//
// >>implementation logic

/* helper to extract hi/lo values securely across architectures */
static inline void _simd_f128_extract_internal(simd_f128 x, double* hi, double* lo) {
    simd_f128_extract(x, hi, lo);
}

/* 
 * prints the 128-bit number as a formatted string to console.
 * we just wrap the to_string function to keep things simple.
 */
void simd_f128_print(simd_f128 x) {
    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), x);
    printf("%s\n", buf);
}

/* 
 * string conversion using iterative fractional extraction.
 * we extract digits one by one to maintain full precision (~32 digits).
 */
void simd_f128_to_string(char* buf, size_t buf_size, simd_f128 x) {
    if (buf == NULL || buf_size == 0) return;

    double hi, lo;
    _simd_f128_extract_internal(x, &hi, &lo);

    /* handle nan/inf cases - standard double logic works here */
    if (isnan(hi)) {
        snprintf(buf, buf_size, "nan");
        return;
    }
    if (isinf(hi)) {
        snprintf(buf, buf_size, hi < 0 ? "-inf" : "inf");
        return;
    }

    /* handle negative values by printing '-' and working with absolute value */
    int is_neg = 0;
    if (hi < 0.0 || (hi == 0.0 && lo < 0.0)) {
        is_neg = 1;
        simd_f128 zero = simd_f128_from_double(0.0);
        x = simd_f128_sub(zero, x);
        _simd_f128_extract_internal(x, &hi, &lo);
    }

    /* get the integer part. we use floor() to be safe with precision. */
    double int_part = floor(hi);
    if (hi == int_part && lo < 0.0) {
        int_part -= 1.0;
    }

    char int_buf[64];
    snprintf(int_buf, sizeof(int_buf), "%.0f", int_part);

    /* 
     * extract fractional digits iteratively.
     * we multiply by 10 and grab the integer part each time.
     */
    simd_f128 f_int = simd_f128_from_double(int_part);
    simd_f128 frac  = simd_f128_sub(x, f_int);
    simd_f128 ten   = simd_f128_from_double(10.0);

    char frac_buf[64] = {0};
    int frac_len = 0;
    
    for (int i = 0; i < 32; i++) {
        frac = simd_f128_mul(frac, ten);
        
        double f_hi, f_lo;
        _simd_f128_extract_internal(frac, &f_hi, &f_lo);
        
        /* tiny epsilon to avoid rounding issues near integer boundaries */
        double digit = floor(f_hi + 1e-30); 
        if (f_hi == digit && f_lo < 0.0) {
            digit -= 1.0;
        }
        
        if (digit < 0.0) digit = 0.0;
        if (digit > 9.0) digit = 9.0;
        
        frac_buf[frac_len++] = '0' + (int)digit;
        frac = simd_f128_sub(frac, simd_f128_from_double(digit));
    }
    frac_buf[frac_len] = '\0';

    /* construct final string output */
    snprintf(buf, buf_size, "%s%s.%s", is_neg ? "-" : "", int_buf, frac_buf);
}

#endif /* SIMD_F128_IMPLEMENTATION */
#endif /* SIMD_F128_IO_H */
