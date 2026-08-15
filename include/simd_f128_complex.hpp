/* simd_f128_complex.hpp
 *
 * c++ wrapper integrating simd_f128 with std::complex.
 * enables seamless usage of std::complex<f128::float128>.
 *
 * updated 2026-08-13
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#ifndef SIMD_F128_COMPLEX_HPP
#define SIMD_F128_COMPLEX_HPP

#include <complex>

#include "simd_f128.hpp"
#include "simd_f128_complex.h"

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
    // raw double-double complex value
    simd_f128_complex data;

    // =========================================================================
    // object lifetime and interoperability
    // =========================================================================
    // provides zero-cost initialization and seamless type coercion between
    // strictly typed c++11 domains, the c-core, and standard library complex types.
    SIMD_F128_DEVICE complex128() {
        data.real = simd_f128_from_double(0.0);
        data.imag = simd_f128_from_double(0.0);
    }

    // constructor from double precision values
    SIMD_F128_DEVICE complex128(double r, double i = 0.0) {
        data.real = simd_f128_from_double(r);
        data.imag = simd_f128_from_double(i);
    }

    // constructor from float128 values
    SIMD_F128_DEVICE complex128(float128 r, float128 i = float128(0.0)) {
        data.real = r.data;
        data.imag = i.data;
    }

    // constructor from raw C struct simd_f128_complex
    SIMD_F128_DEVICE complex128(simd_f128_complex d) : data(d) {}

    // constructor converting from standard library complex (pads lower double precision bits with
    // zero)
    SIMD_F128_DEVICE complex128(const std::complex<double>& c) {
        data.real = simd_f128_from_double(c.real());
        data.imag = simd_f128_from_double(c.imag());
    }

    // conversion operator to standard library complex (truncates double-double precision to double
    // precision)
    SIMD_F128_DEVICE operator std::complex<double>() const {
        double rhi, rlo, ihi, ilo;
        simd_f128_extract(data.real, &rhi, &rlo);
        simd_f128_extract(data.imag, &ihi, &ilo);
        return std::complex<double>(rhi, ihi);
    }

    // retrieve real component
    SIMD_F128_DEVICE float128 real() const {
        return float128(data.real);
    }
    // retrieve imaginary component
    SIMD_F128_DEVICE float128 imag() const {
        return float128(data.imag);
    }

    // =========================================================================
    // algebraic operators
    // =========================================================================
    // maps directly to optimized avx2/wasm c-core routines. all intermediate
    // variables are perfectly bounded to the cpu registers without heap allocation.
    SIMD_F128_DEVICE complex128 operator+(const complex128& b) const {
        return complex128(simd_f128_complex_add(data, b.data));
    }
    SIMD_F128_DEVICE complex128 operator-(const complex128& b) const {
        return complex128(simd_f128_complex_sub(data, b.data));
    }
    SIMD_F128_DEVICE complex128 operator*(const complex128& b) const {
        return complex128(simd_f128_complex_mul(data, b.data));
    }
    SIMD_F128_DEVICE complex128 operator/(const complex128& b) const {
        return complex128(simd_f128_complex_div(data, b.data));
    }

    SIMD_F128_DEVICE complex128& operator+=(const complex128& b) {
        data = simd_f128_complex_add(data, b.data);
        return *this;
    }
    SIMD_F128_DEVICE complex128& operator-=(const complex128& b) {
        data = simd_f128_complex_sub(data, b.data);
        return *this;
    }
    SIMD_F128_DEVICE complex128& operator*=(const complex128& b) {
        data = simd_f128_complex_mul(data, b.data);
        return *this;
    }
    SIMD_F128_DEVICE complex128& operator/=(const complex128& b) {
        data = simd_f128_complex_div(data, b.data);
        return *this;
    }

    // format complex number to std::string
    std::string to_string() const {
        std::string sign = (imag() < float128(0.0)) ? " - " : " + ";
        float128 abs_i = f128::abs(imag());
        return real().to_string() + sign + abs_i.to_string() + "i";
    }
};

// stream output formatting helper
inline std::ostream& operator<<(std::ostream& os, const complex128& val) {
    os << val.to_string();
    return os;
}

// =========================================================================
// complex transcendental & geometric functions
// =========================================================================
// executes high-precision 128-bit operations on the complex plane.
// functions like sin(z) perfectly utilize their mathematical identities
// (e.g., sin(x)*cosh(y) + i*cos(x)*sinh(y)) leveraging the c-core.
inline float128 abs_sqr(const complex128& a) {
    return float128(simd_f128_complex_abs_sqr(a.data));
}

// compute absolute magnitude
inline float128 abs(const complex128& a) {
    return float128(simd_f128_complex_abs(a.data));
}

// compute phase angle
inline float128 arg(const complex128& a) {
    return float128(simd_f128_complex_arg(a.data));
}

// compute complex conjugate
inline complex128 conj(const complex128& a) {
    return complex128(simd_f128_complex_conj(a.data));
}

// compute complex sine: sin(z) = sin(x)*cosh(y) + i*cos(x)*sinh(y)
inline complex128 sin(const complex128& z) {
    float128 x = z.real();
    float128 y = z.imag();
    return complex128(sin(x) * cosh(y), cos(x) * sinh(y));
}

// compute complex cosine: cos(z) = cos(x)*cosh(y) - i*sin(x)*sinh(y)
inline complex128 cos(const complex128& z) {
    float128 x = z.real();
    float128 y = z.imag();
    return complex128(cos(x) * cosh(y), -sin(x) * sinh(y));
}

// compute complex tangent: tan(z) = sin(z) / cos(z)
inline complex128 tan(const complex128& z) {
    return sin(z) / cos(z);
}

// compute complex hyperbolic sine: sinh(z) = sinh(x)*cos(y) + i*cosh(x)*sin(y)
inline complex128 sinh(const complex128& z) {
    float128 x = z.real();
    float128 y = z.imag();
    return complex128(sinh(x) * cos(y), cosh(x) * sin(y));
}

// compute complex hyperbolic cosine: cosh(z) = cosh(x)*cos(y) + i*sinh(x)*sin(y)
inline complex128 cosh(const complex128& z) {
    float128 x = z.real();
    float128 y = z.imag();
    return complex128(cosh(x) * cos(y), sinh(x) * sin(y));
}

// compute complex hyperbolic tangent: tanh(z) = sinh(z) / cosh(z)
inline complex128 tanh(const complex128& z) {
    return sinh(z) / cosh(z);
}

}  // namespace f128

namespace std {

// =========================================================================
// standard library integration (adl)
// =========================================================================
// injects overloads into the std namespace so that float128 can be passed
// directly into templated algorithms and standard containers without modification.
inline f128::complex128 sin(const f128::complex128& z) {
    return f128::sin(z);
}
inline f128::complex128 cos(const f128::complex128& z) {
    return f128::cos(z);
}
inline f128::complex128 tan(const f128::complex128& z) {
    return f128::tan(z);
}
inline f128::complex128 sinh(const f128::complex128& z) {
    return f128::sinh(z);
}
inline f128::complex128 cosh(const f128::complex128& z) {
    return f128::cosh(z);
}
inline f128::complex128 tanh(const f128::complex128& z) {
    return f128::tanh(z);
}
inline f128::float128 abs(const f128::complex128& a) {
    return f128::abs(a);
}
inline f128::float128 arg(const f128::complex128& a) {
    return f128::arg(a);
}
inline f128::complex128 conj(const f128::complex128& a) {
    return f128::conj(a);
}

}  // namespace std

#endif
