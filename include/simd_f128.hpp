/* simd_f128.hpp
 *
 * modern c++ wrapper class for simd_f128 double-double values.
 * provides operator overloading and std::numeric_limits specializations.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#ifndef SIMD_F128_HPP
#define SIMD_F128_HPP

#include <functional>
#include <istream>
#include <limits>
#include <ostream>
#include <string>

#include "simd_f128.h"
#include "simd_f128_consts.h"
#include "simd_f128_io.h"
#include "simd_f128_math.h"
#include "simd_f128_utils.h"

// ██████  ██████  ██████
// ██      ██  ██  ██  ██
// ██      ██████  ██████
// ██      ██      ██
// ██████  ██      ██
//
// >>cpp wrapper class
namespace f128 {

class float128 {
   public:
    // raw double-double simd register data
    simd_f128 data;

    /* constructors:
     * default constructor initializes value to 0.0.
     * from double constructor loads a standard 64-bit float into the high part.
     * from raw type constructor wraps an existing simd_f128 struct/register. */
    SIMD_F128_DEVICE float128() {
        data = simd_f128_from_double(0.0);
    }
    SIMD_F128_DEVICE float128(double d) {
        data = simd_f128_from_double(d);
    }
    SIMD_F128_DEVICE float128(simd_f128 d) : data(d) {}

    /* extraction helper:
     * unpacks the internal 128-bit register into two standard 64-bit doubles.
     * hi contains the primary magnitude, lo contains the accumulated error. */
    SIMD_F128_DEVICE void extract(double& hi, double& lo) const {
        simd_f128_extract(data, &hi, &lo);
    }

    /* arithmetic operator overloading:
     * maps standard c++ mathematical operators to the underlying simd c api.
     * division follows strict ieee-754 semantics regarding division by zero,
     * returning inf or nan directly rather than throwing c++ exceptions,
     * ensuring safe usage in tightly optimized computational loops. */
    SIMD_F128_DEVICE float128 operator+(const float128& b) const {
        return float128(simd_f128_add(data, b.data));
    }
    SIMD_F128_DEVICE float128 operator-(const float128& b) const {
        return float128(simd_f128_sub(data, b.data));
    }
    SIMD_F128_DEVICE float128 operator*(const float128& b) const {
        return float128(simd_f128_mul(data, b.data));
    }
    SIMD_F128_DEVICE float128 operator/(const float128& b) const {
        return float128(simd_f128_div(data, b.data));
    }

    // compound assignment operators:
    // mutates the current object by applying the requested operation.
    SIMD_F128_DEVICE float128& operator+=(const float128& b) {
        data = simd_f128_add(data, b.data);
        return *this;
    }
    SIMD_F128_DEVICE float128& operator-=(const float128& b) {
        data = simd_f128_sub(data, b.data);
        return *this;
    }
    SIMD_F128_DEVICE float128& operator*=(const float128& b) {
        data = simd_f128_mul(data, b.data);
        return *this;
    }
    SIMD_F128_DEVICE float128& operator/=(const float128& b) {
        data = simd_f128_div(data, b.data);
        return *this;
    }

    // operator overloading for comparison operations (checks for nan cases internally)
    SIMD_F128_DEVICE bool operator==(const float128& b) const {
        return simd_f128_eq(data, b.data);
    }
    SIMD_F128_DEVICE bool operator!=(const float128& b) const {
        return !simd_f128_eq(data, b.data);
    }
    SIMD_F128_DEVICE bool operator>(const float128& b) const {
        return simd_f128_gt(data, b.data);
    }
    SIMD_F128_DEVICE bool operator<(const float128& b) const {
        return simd_f128_lt(data, b.data);
    }
    SIMD_F128_DEVICE bool operator>=(const float128& b) const {
        return simd_f128_ge(data, b.data);
    }
    SIMD_F128_DEVICE bool operator<=(const float128& b) const {
        return simd_f128_le(data, b.data);
    }

    // unary negation operator
    SIMD_F128_DEVICE float128 operator-() const {
        return float128(simd_f128_neg(data));
    }

    // format value to std::string
    std::string to_string() const {
        char buf[128];
        simd_f128_to_string(buf, sizeof(buf), data);
        return std::string(buf);
    }
};

// stream output formatting helper
inline std::ostream& operator<<(std::ostream& os, const float128& val) {
    os << val.to_string();
    return os;
}

// stream input parsing helper
inline std::istream& operator>>(std::istream& is, float128& val) {
    std::string s;
    is >> s;
    val.data = simd_f128_from_string(s.c_str());
    return is;
}

// user-defined literal suffix for float128 constants (e.g. "1.234"_f128)
inline float128 operator""_f128(const char* str, std::size_t) {
    return float128(simd_f128_from_string(str));
}

// raw numeric literal suffix for float128 constants (e.g. 1.234_f128 without quotes)
inline float128 operator""_f128(const char* str) {
    return float128(simd_f128_from_string(str));
}

// non-member arithmetic operators for binary double-double math
SIMD_F128_DEVICE inline float128 operator+(double a, const float128& b) {
    return float128(a) + b;
}
SIMD_F128_DEVICE inline float128 operator-(double a, const float128& b) {
    return float128(a) - b;
}
SIMD_F128_DEVICE inline float128 operator*(double a, const float128& b) {
    return float128(a) * b;
}
SIMD_F128_DEVICE inline float128 operator/(double a, const float128& b) {
    return float128(a) / b;
}

SIMD_F128_DEVICE inline bool operator==(double a, const float128& b) {
    return float128(a) == b;
}
SIMD_F128_DEVICE inline bool operator!=(double a, const float128& b) {
    return float128(a) != b;
}
SIMD_F128_DEVICE inline bool operator>(double a, const float128& b) {
    return float128(a) > b;
}
SIMD_F128_DEVICE inline bool operator<(double a, const float128& b) {
    return float128(a) < b;
}
SIMD_F128_DEVICE inline bool operator>=(double a, const float128& b) {
    return float128(a) >= b;
}
SIMD_F128_DEVICE inline bool operator<=(double a, const float128& b) {
    return float128(a) <= b;
}

// global math wrappers mapping to namespace f128 functions
SIMD_F128_DEVICE inline float128 exp(float128 x) {
    return float128(simd_f128_exp(x.data));
}
SIMD_F128_DEVICE inline float128 log(float128 x) {
    return float128(simd_f128_log(x.data));
}
SIMD_F128_DEVICE inline float128 log10(float128 x) {
    return float128(simd_f128_log10(x.data));
}
SIMD_F128_DEVICE inline float128 log2(float128 x) {
    return float128(simd_f128_log2(x.data));
}
SIMD_F128_DEVICE inline float128 pow(float128 b, float128 e) {
    return float128(simd_f128_pow(b.data, e.data));
}
SIMD_F128_DEVICE inline float128 sin(float128 x) {
    return float128(simd_f128_sin(x.data));
}
SIMD_F128_DEVICE inline float128 cos(float128 x) {
    return float128(simd_f128_cos(x.data));
}
SIMD_F128_DEVICE inline float128 sqrt(float128 x) {
    return float128(simd_f128_sqrt(x.data));
}
SIMD_F128_DEVICE inline float128 cbrt(float128 x) {
    return float128(simd_f128_cbrt(x.data));
}
SIMD_F128_DEVICE inline float128 rsqrt(float128 x) {
    return float128(simd_f128_rsqrt(x.data));
}
SIMD_F128_DEVICE inline float128 abs(float128 x) {
    return float128(simd_f128_abs(x.data));
}
SIMD_F128_DEVICE inline float128 erf(float128 x) {
    return float128(simd_f128_erf(x.data));
}
SIMD_F128_DEVICE inline float128 erfc(float128 x) {
    return float128(simd_f128_erfc(x.data));
}
SIMD_F128_DEVICE inline float128 tgamma(float128 x) {
    return float128(simd_f128_tgamma(x.data));
}
SIMD_F128_DEVICE inline float128 j0(float128 x) {
    return float128(simd_f128_j0(x.data));
}
SIMD_F128_DEVICE inline float128 j1(float128 x) {
    return float128(simd_f128_j1(x.data));
}
SIMD_F128_DEVICE inline float128 y0(float128 x) {
    return float128(simd_f128_y0(x.data));
}
SIMD_F128_DEVICE inline float128 y1(float128 x) {
    return float128(simd_f128_y1(x.data));
}
SIMD_F128_DEVICE inline float128 floor(float128 x) {
    return float128(simd_f128_floor(x.data));
}
SIMD_F128_DEVICE inline float128 ceil(float128 x) {
    return float128(simd_f128_ceil(x.data));
}
SIMD_F128_DEVICE inline float128 trunc(float128 x) {
    return float128(simd_f128_trunc(x.data));
}
SIMD_F128_DEVICE inline float128 round(float128 x) {
    return float128(simd_f128_round(x.data));
}
SIMD_F128_DEVICE inline float128 fmod(float128 a, float128 b) {
    return float128(simd_f128_fmod(a.data, b.data));
}
SIMD_F128_DEVICE inline float128 atan(float128 x) {
    return float128(simd_f128_atan(x.data));
}
SIMD_F128_DEVICE inline float128 atan2(float128 y, float128 x) {
    return float128(simd_f128_atan2(y.data, x.data));
}
SIMD_F128_DEVICE inline float128 asin(float128 x) {
    return float128(simd_f128_asin(x.data));
}
SIMD_F128_DEVICE inline float128 acos(float128 x) {
    return float128(simd_f128_acos(x.data));
}
SIMD_F128_DEVICE inline float128 tan(float128 x) {
    return float128(simd_f128_tan(x.data));
}
SIMD_F128_DEVICE inline float128 sinh(float128 x) {
    return float128(simd_f128_sinh(x.data));
}
SIMD_F128_DEVICE inline float128 cosh(float128 x) {
    return float128(simd_f128_cosh(x.data));
}
SIMD_F128_DEVICE inline float128 tanh(float128 x) {
    return float128(simd_f128_tanh(x.data));
}
SIMD_F128_DEVICE inline void sincos(float128 x, float128* s, float128* c) {
    simd_f128_sincos(x.data, &s->data, &c->data);
}
inline bool isnan(float128 x) {
    return simd_f128_isnan(x.data);
}
inline bool isinf(float128 x) {
    return simd_f128_isinf(x.data);
}

// pre-defined mathematical constants in float128 format
inline const float128 pi(SIMD_F128_PI);
inline const float128 e(SIMD_F128_E);
inline const float128 sqrt2(SIMD_F128_SQRT2);
inline const float128 ln2(SIMD_F128_LN2);

}  // namespace f128

namespace std {

// std::numeric_limits specialization for float128
template <>
class numeric_limits<f128::float128> {
   public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = false;
    static constexpr bool has_denorm = std::numeric_limits<double>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<double>::has_denorm_loss;
    static constexpr float_round_style round_style = std::round_to_nearest;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 106;   // 106 bits for double-double mantissa
    static constexpr int digits10 = 31;  // ~31-32 decimal digits
    static constexpr int max_digits10 = 33;
    static constexpr int radix = 2;
    static constexpr int min_exponent = std::numeric_limits<double>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<double>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<double>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<double>::max_exponent10;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;

    static f128::float128 min() noexcept {
        return f128::float128(simd_f128_from_hi_lo(2.2250738585072014e-308, 0.0));
    }  // normalized min
    static f128::float128 lowest() noexcept {
        return -max();
    }
    static f128::float128 max() noexcept {
        return f128::float128(simd_f128_from_hi_lo(1.7976931348623157e+308, 0.0));
    }
    static f128::float128 epsilon() noexcept {
        return f128::float128(simd_f128_from_hi_lo(4.9303806576313238e-32, 0.0));
    }  // 2^-104
    static f128::float128 round_error() noexcept {
        return f128::float128(simd_f128_from_hi_lo(0.5, 0.0));
    }
    static f128::float128 infinity() noexcept {
        return f128::float128(simd_f128_from_hi_lo(INFINITY, 0.0));
    }
    static f128::float128 quiet_NaN() noexcept {
        return f128::float128(simd_f128_from_hi_lo(NAN, 0.0));
    }
    static f128::float128 signaling_NaN() noexcept {
        return f128::float128(simd_f128_from_hi_lo(NAN, 0.0));
    }
    static f128::float128 denorm_min() noexcept {
        return f128::float128(simd_f128_from_hi_lo(4.9406564584124654e-324, 0.0));
    }
};

// std::hash specialization for float128
template <>
struct hash<f128::float128> {
    size_t operator()(const f128::float128& val) const noexcept {
        double hi, lo;
        val.extract(hi, lo);
        size_t h1 = std::hash<double>{}(hi);
        size_t h2 = std::hash<double>{}(lo);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

// std namespace overloads to allow standard template resolution and adl integration
inline f128::float128 exp(f128::float128 x) {
    return f128::exp(x);
}
inline f128::float128 log(f128::float128 x) {
    return f128::log(x);
}
inline f128::float128 log10(f128::float128 x) {
    return f128::log10(x);
}
inline f128::float128 log2(f128::float128 x) {
    return f128::log2(x);
}
inline f128::float128 pow(f128::float128 b, f128::float128 e) {
    return f128::pow(b, e);
}
inline f128::float128 sin(f128::float128 x) {
    return f128::sin(x);
}
inline f128::float128 cos(f128::float128 x) {
    return f128::cos(x);
}
inline f128::float128 tan(f128::float128 x) {
    return f128::tan(x);
}
inline f128::float128 sinh(f128::float128 x) {
    return f128::sinh(x);
}
inline f128::float128 cosh(f128::float128 x) {
    return f128::cosh(x);
}
inline f128::float128 tanh(f128::float128 x) {
    return f128::tanh(x);
}
inline f128::float128 sqrt(f128::float128 x) {
    return f128::sqrt(x);
}
inline f128::float128 cbrt(f128::float128 x) {
    return f128::cbrt(x);
}
inline f128::float128 erf(f128::float128 x) {
    return f128::erf(x);
}
inline f128::float128 erfc(f128::float128 x) {
    return f128::erfc(x);
}
inline f128::float128 tgamma(f128::float128 x) {
    return f128::tgamma(x);
}
inline f128::float128 j0(f128::float128 x) {
    return f128::j0(x);
}
inline f128::float128 j1(f128::float128 x) {
    return f128::j1(x);
}
inline f128::float128 y0(f128::float128 x) {
    return f128::y0(x);
}
inline f128::float128 y1(f128::float128 x) {
    return f128::y1(x);
}
inline f128::float128 abs(f128::float128 x) {
    return f128::abs(x);
}
inline f128::float128 floor(f128::float128 x) {
    return f128::floor(x);
}
inline f128::float128 ceil(f128::float128 x) {
    return f128::ceil(x);
}
inline f128::float128 trunc(f128::float128 x) {
    return f128::trunc(x);
}
inline f128::float128 round(f128::float128 x) {
    return f128::round(x);
}
inline f128::float128 fmod(f128::float128 a, f128::float128 b) {
    return f128::fmod(a, b);
}
inline f128::float128 atan(f128::float128 x) {
    return f128::atan(x);
}
inline f128::float128 atan2(f128::float128 y, f128::float128 x) {
    return f128::atan2(y, x);
}
inline f128::float128 asin(f128::float128 x) {
    return f128::asin(x);
}
inline f128::float128 acos(f128::float128 x) {
    return f128::acos(x);
}
inline bool isnan(f128::float128 x) {
    return f128::isnan(x);
}
inline bool isinf(f128::float128 x) {
    return f128::isinf(x);
}

}  // namespace std

#endif
