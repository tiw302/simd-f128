// updated 2026-05-23

#ifndef SIMD_F128_EIGEN_HPP
#define SIMD_F128_EIGEN_HPP

#include "simd_f128.hpp"
#include "simd_f128_complex.hpp"
#include <Eigen/Core>

// ███████ ██  ██████  ███████ ███    ██ 
// ██      ██ ██       ██      ████   ██ 
// █████   ██ ██   ███ █████   ██ ██  ██ 
// ██      ██ ██    ██ ██      ██  ██ ██ 
// ███████ ██  ██████  ███████ ██   ████ 
//
// >>eigen matrix traits integration

namespace Eigen {

// specialize eigen::numtraits for float128 type
template<> struct NumTraits<f128::float128>
 : NumTraits<double> 
{
  typedef f128::float128 Real;
  typedef f128::float128 NonInteger;
  typedef f128::float128 Nested;
  
  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    // costs are high because double-double arithmetic requires multiple instruction
    // sequences (e.g. 20+ floating-point operations for a single multiplication)
    ReadCost = 2,
    AddCost = 10,
    MulCost = 20
  };
};

// specialize eigen::numtraits for complex128 type
template<> struct NumTraits<f128::complex128>
 : NumTraits<std::complex<double>>
{
  typedef f128::float128 Real;
  typedef f128::complex128 NonInteger;
  typedef f128::complex128 Nested;
  
  enum {
    IsComplex = 1,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    // complex operations are approximately four times more expensive than real operations
    ReadCost = 4,
    AddCost = 20,
    MulCost = 80
  };
};

namespace numext {
    // map eigen's math extension functions to the high-precision float128 functions
    inline f128::float128 exp(const f128::float128& x) { return f128::exp(x); }
    inline f128::float128 log(const f128::float128& x) { return f128::log(x); }
    inline f128::float128 sin(const f128::float128& x) { return f128::sin(x); }
    inline f128::float128 cos(const f128::float128& x) { return f128::cos(x); }
    inline f128::float128 sqrt(const f128::float128& x) { return f128::sqrt(x); }
    inline f128::float128 abs(const f128::float128& x) { return f128::abs(x); }
    inline f128::float128 pow(const f128::float128& x, const f128::float128& y) { return f128::pow(x, y); }
}

} // namespace Eigen

#endif // simd_f128_eigen_hpp
