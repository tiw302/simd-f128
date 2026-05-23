// updated 2026-05-23

#ifndef SIMD_F128_COMPLEX_HPP
#define SIMD_F128_COMPLEX_HPP

#include "simd_f128.hpp"
#include "simd_f128_complex.h"
#include <complex>

// ██████  ██████  ██████  
// ██      ██  ██  ██  ██ 
// ██      ██████  ██████ 
// ██      ██      ██     
// ██████  ██      ██     
//
// >>cpp wrapper class for complex numbers

namespace f128 {

class complex128 {
public:
    simd_f128_complex data;

    SIMD_F128_DEVICE complex128() {
        data.real = simd_f128_from_double(0.0);
        data.imag = simd_f128_from_double(0.0);
    }
    
    SIMD_F128_DEVICE complex128(double r, double i = 0.0) {
        data.real = simd_f128_from_double(r);
        data.imag = simd_f128_from_double(i);
    }

    SIMD_F128_DEVICE complex128(float128 r, float128 i = float128(0.0)) {
        data.real = r.data;
        data.imag = i.data;
    }
    
    SIMD_F128_DEVICE complex128(simd_f128_complex d) : data(d) {}

    // bridge from standard library (pads lower 64 bits with 0)
    SIMD_F128_DEVICE complex128(const std::complex<double>& c) {
        data.real = simd_f128_from_double(c.real());
        data.imag = simd_f128_from_double(c.imag());
    }

    // bridge back to standard library (truncates and loses 128-bit precision)
    SIMD_F128_DEVICE operator std::complex<double>() const {
        double rhi, rlo, ihi, ilo;
        simd_f128_extract(data.real, &rhi, &rlo);
        simd_f128_extract(data.imag, &ihi, &ilo);
        return std::complex<double>(rhi, ihi);
    }

    SIMD_F128_DEVICE float128 real() const { return float128(data.real); }
    SIMD_F128_DEVICE float128 imag() const { return float128(data.imag); }

    SIMD_F128_DEVICE complex128 operator+(const complex128& b) const { return complex128(simd_f128_complex_add(data, b.data)); }
    SIMD_F128_DEVICE complex128 operator-(const complex128& b) const { return complex128(simd_f128_complex_sub(data, b.data)); }
    SIMD_F128_DEVICE complex128 operator*(const complex128& b) const { return complex128(simd_f128_complex_mul(data, b.data)); }

    SIMD_F128_DEVICE complex128& operator+=(const complex128& b) { data = simd_f128_complex_add(data, b.data); return *this; }
    SIMD_F128_DEVICE complex128& operator-=(const complex128& b) { data = simd_f128_complex_sub(data, b.data); return *this; }
    SIMD_F128_DEVICE complex128& operator*=(const complex128& b) { data = simd_f128_complex_mul(data, b.data); return *this; }

    std::string to_string() const {
        std::string sign = (imag() < float128(0.0)) ? " - " : " + ";
        float128 abs_i = f128::abs(imag());
        return real().to_string() + sign + abs_i.to_string() + "i";
    }
};

inline std::ostream& operator<<(std::ostream& os, const complex128& val) {
    os << val.to_string();
    return os;
}

inline float128 abs_sqr(const complex128& a) {
    return float128(simd_f128_complex_abs_sqr(a.data));
}

} // namespace f128

#endif /* simd_f128_complex_hpp */
