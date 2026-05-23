// updated 2026-05-23

#ifndef SIMD_F128_HPP
#define SIMD_F128_HPP

#include "simd_f128.h"
#include "simd_f128_consts.h"
#include "simd_f128_io.h"
#include "simd_f128_utils.h"
#include "simd_f128_math.h"
#include <iostream>
#include <string>
#include <stdexcept>

// ██████  ██████  ██████  
// ██      ██  ██  ██  ██ 
// ██      ██████  ██████ 
// ██      ██      ██     
// ██████  ██      ██     
//
// >>cpp wrapper class

namespace f128 {

/* 
 * the float128 class wraps our c simd_f128 type to make it 
 * feel like a first-class citizen in c++. 
 * no more simd_f128_add(a, b) – just use a + b.
 */
class float128 {
public:
    simd_f128 data;

    // constructors
    SIMD_F128_DEVICE float128() { data = simd_f128_from_double(0.0); }
    SIMD_F128_DEVICE float128(double d) { data = simd_f128_from_double(d); }
    SIMD_F128_DEVICE float128(simd_f128 d) : data(d) {}

    // conversion back to hi/lo
    SIMD_F128_DEVICE void extract(double& hi, double& lo) const {
        simd_f128_extract(data, &hi, &lo);
    }

    // operator overloading (arithmetic)
    SIMD_F128_DEVICE float128 operator+(const float128& b) const { return float128(simd_f128_add(data, b.data)); }
    SIMD_F128_DEVICE float128 operator-(const float128& b) const { return float128(simd_f128_sub(data, b.data)); }
    SIMD_F128_DEVICE float128 operator*(const float128& b) const { return float128(simd_f128_mul(data, b.data)); }
    SIMD_F128_DEVICE float128 operator/(const float128& b) const { 
        // exceptions are disabled on cuda/hip because device code doesn't support them
#if defined(SIMD_F128_EXCEPTIONS) && !defined(__CUDA_ARCH__) && !defined(__HIP_DEVICE_COMPILE__)
        if (b == float128(0.0)) throw std::domain_error("Division by zero in float128");
#endif
        return float128(simd_f128_div(data, b.data)); 
    }

    SIMD_F128_DEVICE float128& operator+=(const float128& b) { data = simd_f128_add(data, b.data); return *this; }
    SIMD_F128_DEVICE float128& operator-=(const float128& b) { data = simd_f128_sub(data, b.data); return *this; }
    SIMD_F128_DEVICE float128& operator*=(const float128& b) { data = simd_f128_mul(data, b.data); return *this; }
    SIMD_F128_DEVICE float128& operator/=(const float128& b) { data = simd_f128_div(data, b.data); return *this; }

    // operator overloading (comparison)
    SIMD_F128_DEVICE bool operator==(const float128& b) const { return simd_f128_eq(data, b.data); }
    SIMD_F128_DEVICE bool operator!=(const float128& b) const { return !simd_f128_eq(data, b.data); }
    SIMD_F128_DEVICE bool operator>(const float128& b) const { return simd_f128_gt(data, b.data); }
    SIMD_F128_DEVICE bool operator<(const float128& b) const { return simd_f128_lt(data, b.data); }
    SIMD_F128_DEVICE bool operator>=(const float128& b) const { return simd_f128_ge(data, b.data); }
    SIMD_F128_DEVICE bool operator<=(const float128& b) const { return simd_f128_le(data, b.data); }

    // unary operators
    SIMD_F128_DEVICE float128 operator-() const { 
        double hi, lo;
        simd_f128_extract(data, &hi, &lo);
        return float128(simd_f128_sub(simd_f128_from_double(0.0), data)); 
    }

    // helper for printing
    std::string to_string() const {
        char buf[128];
        simd_f128_to_string(buf, sizeof(buf), data);
        return std::string(buf);
    }
};

// ostream integration so we can do std::cout << val;
inline std::ostream& operator<<(std::ostream& os, const float128& val) {
    os << val.to_string();
    return os;
}

// istream integration for std::cin >> val;
inline std::istream& operator>>(std::istream& is, float128& val) {
    std::string s;
    is >> s;
    val.data = simd_f128_from_string(s.c_str());
    return is;
}

// user-defined literal for easy instantiation
// e.g. "3.1415926535897932384626433832795"_f128
SIMD_F128_DEVICE inline float128 operator""_f128(const char* str, std::size_t) {
    return float128(simd_f128_from_string(str));
}

// shorthand math functions for the namespace
SIMD_F128_DEVICE inline float128 exp(float128 x) { return float128(simd_f128_exp(x.data)); }
SIMD_F128_DEVICE inline float128 log(float128 x) { 
#if defined(SIMD_F128_EXCEPTIONS) && !defined(__CUDA_ARCH__) && !defined(__HIP_DEVICE_COMPILE__)
    if (x <= float128(0.0)) throw std::domain_error("Logarithm of non-positive number");
#endif
    return float128(simd_f128_log(x.data)); 
}
SIMD_F128_DEVICE inline float128 pow(float128 b, float128 e) { return float128(simd_f128_pow(b.data, e.data)); }
SIMD_F128_DEVICE inline float128 sin(float128 x) { return float128(simd_f128_sin(x.data)); }
SIMD_F128_DEVICE inline float128 cos(float128 x) { return float128(simd_f128_cos(x.data)); }
SIMD_F128_DEVICE inline float128 sqrt(float128 x) { 
#if defined(SIMD_F128_EXCEPTIONS) && !defined(__CUDA_ARCH__) && !defined(__HIP_DEVICE_COMPILE__)
    if (x < float128(0.0)) throw std::domain_error("Square root of negative number");
#endif
    return float128(simd_f128_sqrt(x.data)); 
}
SIMD_F128_DEVICE inline float128 rsqrt(float128 x) { 
#if defined(SIMD_F128_EXCEPTIONS) && !defined(__CUDA_ARCH__) && !defined(__HIP_DEVICE_COMPILE__)
    if (x <= float128(0.0)) throw std::domain_error("Inverse square root of non-positive number");
#endif
    return float128(simd_f128_rsqrt(x.data)); 
}
SIMD_F128_DEVICE inline float128 abs(float128 x) { return float128(simd_f128_abs(x.data)); }
SIMD_F128_DEVICE inline float128 floor(float128 x) { return float128(simd_f128_floor(x.data)); }
SIMD_F128_DEVICE inline float128 ceil(float128 x) { return float128(simd_f128_ceil(x.data)); }
SIMD_F128_DEVICE inline float128 trunc(float128 x) { return float128(simd_f128_trunc(x.data)); }
SIMD_F128_DEVICE inline float128 round(float128 x) { return float128(simd_f128_round(x.data)); }
SIMD_F128_DEVICE inline float128 fmod(float128 a, float128 b) { return float128(simd_f128_fmod(a.data, b.data)); }
SIMD_F128_DEVICE inline float128 atan(float128 x) { return float128(simd_f128_atan(x.data)); }
SIMD_F128_DEVICE inline float128 atan2(float128 y, float128 x) { return float128(simd_f128_atan2(y.data, x.data)); }
SIMD_F128_DEVICE inline float128 asin(float128 x) { return float128(simd_f128_asin(x.data)); }
SIMD_F128_DEVICE inline float128 acos(float128 x) { return float128(simd_f128_acos(x.data)); }
inline bool isnan(float128 x) { return simd_f128_isnan(x.data); }
inline bool isinf(float128 x) { return simd_f128_isinf(x.data); }

// constants in cpp style
const float128 pi(SIMD_F128_PI);
const float128 e(SIMD_F128_E);
const float128 sqrt2(SIMD_F128_SQRT2);
const float128 ln2(SIMD_F128_LN2);

} // namespace f128

#endif /* simd_f128_hpp */
