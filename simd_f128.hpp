#ifndef SIMD_F128_HPP
#define SIMD_F128_HPP

#include "simd_f128.h"
#include "simd_f128_consts.h"
#include "simd_f128_io.h"
#include "simd_f128_utils.h"
#include "simd_f128_math.h"
#include <iostream>
#include <string>

// ██████  ██████  ██████  
// ██      ██  ██  ██  ██ 
// ██      ██████  ██████ 
// ██      ██      ██     
// ██████  ██      ██     
//
// >>cpp wrapper class

namespace f128 {

/* 
 * the float128 class wraps our C simd_f128 type to make it 
 * feel like a first-class citizen in C++. 
 * no more simd_f128_add(a, b) – just use a + b.
 */
class float128 {
public:
    simd_f128 data;

    // constructors
    float128() { data = simd_f128_from_double(0.0); }
    float128(double d) { data = simd_f128_from_double(d); }
    float128(simd_f128 d) : data(d) {}

    // conversion back to hi/lo
    void extract(double& hi, double& lo) const {
        simd_f128_extract(data, &hi, &lo);
    }

    // operator overloading (arithmetic)
    float128 operator+(const float128& b) const { return float128(simd_f128_add(data, b.data)); }
    float128 operator-(const float128& b) const { return float128(simd_f128_sub(data, b.data)); }
    float128 operator*(const float128& b) const { return float128(simd_f128_mul(data, b.data)); }
    float128 operator/(const float128& b) const { return float128(simd_f128_div(data, b.data)); }

    float128& operator+=(const float128& b) { data = simd_f128_add(data, b.data); return *this; }
    float128& operator-=(const float128& b) { data = simd_f128_sub(data, b.data); return *this; }
    float128& operator*=(const float128& b) { data = simd_f128_mul(data, b.data); return *this; }
    float128& operator/=(const float128& b) { data = simd_f128_div(data, b.data); return *this; }

    // operator overloading (comparison)
    bool operator==(const float128& b) const { return simd_f128_eq(data, b.data); }
    bool operator!=(const float128& b) const { return !simd_f128_eq(data, b.data); }
    bool operator>(const float128& b) const { return simd_f128_gt(data, b.data); }
    bool operator<(const float128& b) const { return simd_f128_lt(data, b.data); }
    bool operator>=(const float128& b) const { return simd_f128_ge(data, b.data); }
    bool operator<=(const float128& b) const { return simd_f128_le(data, b.data); }

    // unary operators
    float128 operator-() const { 
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

// shorthand math functions for the namespace
inline float128 exp(float128 x) { return float128(simd_f128_exp(x.data)); }
inline float128 log(float128 x) { return float128(simd_f128_log(x.data)); }
inline float128 pow(float128 b, float128 e) { return float128(simd_f128_pow(b.data, e.data)); }
inline float128 sin(float128 x) { return float128(simd_f128_sin(x.data)); }
inline float128 cos(float128 x) { return float128(simd_f128_cos(x.data)); }
inline float128 sqrt(float128 x) { return float128(simd_f128_sqrt(x.data)); }
inline float128 abs(float128 x) { return float128(simd_f128_abs(x.data)); }

// constants in cpp style
const float128 pi(SIMD_F128_PI);
const float128 e(SIMD_F128_E);
const float128 sqrt2(SIMD_F128_SQRT2);
const float128 ln2(SIMD_F128_LN2);

} // namespace f128

#endif /* SIMD_F128_HPP */
