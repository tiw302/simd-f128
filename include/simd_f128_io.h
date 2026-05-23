// updated 2026-05-23

#ifndef SIMD_F128_IO_H
#define SIMD_F128_IO_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "simd_f128.h"


// ███████ ████████ ██████  ██ ███    ██  ██████  
// ██         ██    ██   ██ ██ ████   ██ ██       
// ███████    ██    ██████  ██ ██ ██  ██ ██   ███ 
//      ██    ██    ██   ██ ██ ██  ██ ██ ██    ██ 
// ███████    ██    ██   ██ ██ ██   ████  ██████  
//
// >>string parsing api

#ifdef __cplusplus
extern "C" {
#endif

// prints the 128-bit number to stdout
void simd_f128_print(simd_f128 x);

// converts the number to a string buffer
void simd_f128_to_string(char* buf, size_t buf_size, simd_f128 x);

// parses a string into a 128-bit number
simd_f128 simd_f128_from_string(const char* str);

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

// safe hi/lo extract
static inline void _simd_f128_extract_internal(simd_f128 x, double* hi, double* lo) {
    simd_f128_extract(x, hi, lo);
}

// wrap to_string and dump to stdout
void simd_f128_print(simd_f128 x) {
    char buf[128];
    simd_f128_to_string(buf, sizeof(buf), x);
    printf("%s\n", buf);
}

// digit-by-digit extraction (~32 digits)
void simd_f128_to_string(char* buf, size_t buf_size, simd_f128 x) {
    if (buf == NULL || buf_size == 0) return;

    double hi, lo;
    _simd_f128_extract_internal(x, &hi, &lo);

    // handle nan/inf
    if (isnan(hi)) {
        snprintf(buf, buf_size, "nan");
        return;
    }
    if (isinf(hi)) {
        snprintf(buf, buf_size, hi < 0 ? "-inf" : "inf");
        return;
    }

    // handle negatives
    int is_neg = 0;
    if (hi < 0.0 || (hi == 0.0 && lo < 0.0)) {
        is_neg = 1;
        simd_f128 zero = simd_f128_from_double(0.0);
        x = simd_f128_sub(zero, x);
        _simd_f128_extract_internal(x, &hi, &lo);
    }

    // grab int part (use floor just in case)
    double int_part = floor(hi);
    if (hi == int_part && lo < 0.0) {
        int_part -= 1.0;
    }

    char int_buf[64];
    snprintf(int_buf, sizeof(int_buf), "%.0f", int_part);

    // multiply by 10, grab int part, repeat
    simd_f128 f_int = simd_f128_from_double(int_part);
    simd_f128 frac  = simd_f128_sub(x, f_int);
    simd_f128 ten   = simd_f128_from_double(10.0);

    char frac_buf[64] = {0};
    int frac_len = 0;
    
    for (int i = 0; i < 32; i++) {
        frac = simd_f128_mul(frac, ten);
        
        double f_hi, f_lo;
        _simd_f128_extract_internal(frac, &f_hi, &f_lo);
        
        // tiny epsilon to avoid rounding issues near integer boundaries
        // e.g. when f_hi is 8.9999999999999998, we want 9, not 8
        double digit = floor(f_hi + 1e-30); 
        if (f_hi == digit && f_lo < 0.0) {
            digit -= 1.0;
        }
        
        if (digit < 0.0) digit = 0.0;
        if (digit > 9.0) digit = 9.0;
        
        frac_buf[frac_len++] = (char)('0' + (int)digit);
        frac = simd_f128_sub(frac, simd_f128_from_double(digit));
    }
    frac_buf[frac_len] = '\0';

    // construct final string output
    snprintf(buf, buf_size, "%s%s.%s", is_neg ? "-" : "", int_buf, frac_buf);
}

// parse string natively (avoids float truncation)
simd_f128 simd_f128_from_string(const char* str) {
    simd_f128 res = simd_f128_from_double(0.0);
    if (!str) return res;

    // skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;

    int is_neg = 0;
    if (*str == '-') {
        is_neg = 1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    if (strncmp(str, "nan", 3) == 0 || strncmp(str, "NAN", 3) == 0) {
        return simd_f128_from_double(NAN);
    }
    if (strncmp(str, "inf", 3) == 0 || strncmp(str, "INF", 3) == 0) {
        return simd_f128_from_double(is_neg ? -INFINITY : INFINITY);
    }

    simd_f128 ten = simd_f128_from_double(10.0);
    
    // int part
    while (*str >= '0' && *str <= '9') {
        double d = (double)(*str - '0');
        res = simd_f128_add(simd_f128_mul(res, ten), simd_f128_from_double(d));
        str++;
    }

    // frac part
    if (*str == '.') {
        str++;
        simd_f128 frac_mult = simd_f128_from_double(1.0);
        while (*str >= '0' && *str <= '9') {
            // divide by 10 for each fractional decimal place
            frac_mult = simd_f128_div(frac_mult, ten);
            double d = (double)(*str - '0');
            // accumulate: res = res + d * (10^-n)
            // note: iterative parsing may accumulate tiny errors for very long strings
            res = simd_f128_add(res, simd_f128_mul(simd_f128_from_double(d), frac_mult));
            str++;
        }
    }

    // exp part
    if (*str == 'e' || *str == 'E') {
        str++;
        int exp_neg = 0;
        if (*str == '-') {
            exp_neg = 1;
            str++;
        } else if (*str == '+') {
            str++;
        }
        int exp_val = 0;
        while (*str >= '0' && *str <= '9') {
            exp_val = exp_val * 10 + (*str - '0');
            str++;
        }
        
        // exp scaling
        simd_f128 exp_mult = simd_f128_from_double(1.0);
        for (int i = 0; i < exp_val; i++) {
            exp_mult = simd_f128_mul(exp_mult, ten);
        }
        
        if (exp_neg) {
            res = simd_f128_div(res, exp_mult);
        } else {
            res = simd_f128_mul(res, exp_mult);
        }
    }

    if (is_neg) {
        simd_f128 zero = simd_f128_from_double(0.0);
        res = simd_f128_sub(zero, res);
    }

    return res;
}

#endif // simd_f128_implementation
#endif // simd_f128_io_h
