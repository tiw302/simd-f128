/* lib.rs
 *
 * high-performance 128-bit (double-double) arithmetic for rust.
 * safe abstractions over the c-core. implements std::ops for native feel.
 *
 * project url: https://github.com/tiw302/simd-f128
 *
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

use std::ops::{Add, Sub, Mul, Div, AddAssign, SubAssign, MulAssign, DivAssign, Neg};
use std::cmp::Ordering;

/* 128-bit floating point type.
 * strictly aligned to 16 bytes for avx2/wasm simd128 compatibility over ffi. */
#[repr(C, align(16))]
#[derive(Copy, Clone, Debug)]
pub struct Float128 {
    data: [f64; 2],
}

/* 128-bit complex floating point type.
 * aligned to 16 bytes for simd compatibility. */
#[repr(C, align(16))]
#[derive(Copy, Clone, Debug)]
pub struct Complex128 {
    data: [f64; 4],
}

/* ffi bindings.
 * uses pointers instead of passing structs by value to avoid c abi issues. */
extern "C" {
    fn rs_simd_f128_add(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_sub(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_neg(a: *const f64, out: *mut f64);
    fn rs_simd_f128_mul(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_div(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_sqrt(a: *const f64, out: *mut f64);
    fn rs_simd_f128_exp(a: *const f64, out: *mut f64);
    fn rs_simd_f128_log(a: *const f64, out: *mut f64);
    fn rs_simd_f128_sin(a: *const f64, out: *mut f64);
    fn rs_simd_f128_cos(a: *const f64, out: *mut f64);
    fn rs_simd_f128_from_double(d: f64, out: *mut f64);
    fn rs_simd_f128_from_string(str: *const std::os::raw::c_char, out: *mut f64);
    fn rs_simd_f128_cmp(a: *const f64, b: *const f64) -> std::os::raw::c_int;
    fn rs_simd_f128_pow(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_abs(a: *const f64, out: *mut f64);
    fn rs_simd_f128_floor(a: *const f64, out: *mut f64);
    fn rs_simd_f128_ceil(a: *const f64, out: *mut f64);
    fn rs_simd_f128_round(a: *const f64, out: *mut f64);
    fn rs_simd_f128_trunc(a: *const f64, out: *mut f64);
    fn rs_simd_f128_fmod(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_atan(a: *const f64, out: *mut f64);
    fn rs_simd_f128_atan2(y: *const f64, x: *const f64, out: *mut f64);
    fn rs_simd_f128_asin(a: *const f64, out: *mut f64);
    fn rs_simd_f128_acos(a: *const f64, out: *mut f64);
    fn rs_simd_f128_tan(a: *const f64, out: *mut f64);
    fn rs_simd_f128_sinh(a: *const f64, out: *mut f64);
    fn rs_simd_f128_cosh(a: *const f64, out: *mut f64);
    fn rs_simd_f128_tanh(a: *const f64, out: *mut f64);
    fn rs_simd_f128_to_string(a: *const f64, buf: *mut std::os::raw::c_char, buf_size: usize);
    fn rs_simd_f128_const_pi(out: *mut f64);
    fn rs_simd_f128_const_e(out: *mut f64);
    fn rs_simd_f128_const_sqrt2(out: *mut f64);
    fn rs_simd_f128_const_ln2(out: *mut f64);

    fn rs_simd_f128_complex_add(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_sub(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_mul(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_div(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_abs_sqr(a: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_abs(a: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_arg(a: *const f64, out: *mut f64);
    fn rs_simd_f128_complex_conj(a: *const f64, out: *mut f64);
}

// =========================================================================
// float128 implementation
// =========================================================================
impl Float128 {
    pub fn new(val: f64) -> Self {
        let mut out = [0.0; 2];
        unsafe {
            rs_simd_f128_from_double(val, out.as_mut_ptr());
        }
        Float128 { data: out }
    }

    pub fn extract(&self) -> (f64, f64) {
        (self.data[0], self.data[1])
    }

    // =========================================================================
    // transcendental and geometric math functions
    // =========================================================================
    pub fn sqrt(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_sqrt(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn exp(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_exp(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn log(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_log(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn sin(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_sin(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn cos(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_cos(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn tan(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_tan(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn sinh(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_sinh(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn cosh(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_cosh(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn tanh(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_tanh(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn pow(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_pow(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn abs(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_abs(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn floor(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_floor(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn ceil(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_ceil(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn round(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_round(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn trunc(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_trunc(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn fmod(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_fmod(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn atan(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_atan(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn atan2(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_atan2(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn asin(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_asin(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn acos(self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_acos(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn pi() -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_const_pi(out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn e() -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_const_e(out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn sqrt2() -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_const_sqrt2(out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn ln2() -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_const_ln2(out.as_mut_ptr()); }
        Float128 { data: out }
    }
}

// =========================================================================
// complex128 implementation
// =========================================================================
impl Complex128 {
    pub fn new() -> Self {
        Complex128 { data: [0.0; 4] }
    }

    pub fn new_f128(real: Float128, imag: Float128) -> Self {
        let (rhi, rlo) = real.extract();
        let (ihi, ilo) = imag.extract();
        Complex128 { data: [rhi, rlo, ihi, ilo] }
    }

    pub fn new_f64(real: f64, imag: f64) -> Self {
        Self::new_f128(Float128::new(real), Float128::new(imag))
    }

    pub fn real(&self) -> Float128 {
        Float128 { data: [self.data[0], self.data[1]] }
    }

    pub fn imag(&self) -> Float128 {
        Float128 { data: [self.data[2], self.data[3]] }
    }

    pub fn abs_sqr(self) -> Float128 {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_complex_abs_sqr(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn abs(self) -> Float128 {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_complex_abs(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn arg(self) -> Float128 {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_complex_arg(self.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }

    pub fn conj(self) -> Self {
        let mut out = [0.0; 4];
        unsafe { rs_simd_f128_complex_conj(self.data.as_ptr(), out.as_mut_ptr()); }
        Complex128 { data: out }
    }

    pub fn sin(self) -> Self {
        let x = self.real();
        let y = self.imag();
        Self::new_f128(x.sin() * y.cosh(), x.cos() * y.sinh())
    }

    pub fn cos(self) -> Self {
        let x = self.real();
        let y = self.imag();
        Self::new_f128(x.cos() * y.cosh(), -(x.sin() * y.sinh()))
    }

    pub fn tan(self) -> Self {
        self.sin() / self.cos()
    }

    pub fn sinh(self) -> Self {
        let x = self.real();
        let y = self.imag();
        Self::new_f128(x.sinh() * y.cos(), x.cosh() * y.sin())
    }

    pub fn cosh(self) -> Self {
        let x = self.real();
        let y = self.imag();
        Self::new_f128(x.cosh() * y.cos(), x.sinh() * y.sin())
    }

    pub fn tanh(self) -> Self {
        self.sinh() / self.cosh()
    }
}

// =========================================================================
// operator overloading macros
// =========================================================================
macro_rules! impl_arithmetic_ops {
    ($type:ident, $trait:ident, $method:ident, $ffi_func:ident, $out_size:expr) => {
        impl $trait for $type {
            type Output = Self;
            fn $method(self, other: Self) -> Self {
                let mut out = [0.0; $out_size];
                unsafe { $ffi_func(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
                $type { data: out }
            }
        }
        impl $trait for &$type {
            type Output = $type;
            fn $method(self, other: Self) -> $type {
                (*self).$method(*other)
            }
        }
        impl $trait<$type> for &$type {
            type Output = $type;
            fn $method(self, other: $type) -> $type {
                (*self).$method(other)
            }
        }
        impl $trait<&$type> for $type {
            type Output = $type;
            fn $method(self, other: &$type) -> $type {
                self.$method(*other)
            }
        }
    };
}

macro_rules! impl_assign_ops {
    ($type:ident, $trait:ident, $method:ident, $op_method:ident) => {
        impl $trait for $type {
            fn $method(&mut self, other: Self) {
                *self = self.$op_method(other);
            }
        }
        impl $trait<&$type> for $type {
            fn $method(&mut self, other: &$type) {
                *self = self.$op_method(*other);
            }
        }
    };
}

// float128 basic arithmetic
impl_arithmetic_ops!(Float128, Add, add, rs_simd_f128_add, 2);
impl_arithmetic_ops!(Float128, Sub, sub, rs_simd_f128_sub, 2);
impl_arithmetic_ops!(Float128, Mul, mul, rs_simd_f128_mul, 2);
impl_arithmetic_ops!(Float128, Div, div, rs_simd_f128_div, 2);

impl_assign_ops!(Float128, AddAssign, add_assign, add);
impl_assign_ops!(Float128, SubAssign, sub_assign, sub);
impl_assign_ops!(Float128, MulAssign, mul_assign, mul);
impl_assign_ops!(Float128, DivAssign, div_assign, div);

impl_arithmetic_ops!(Complex128, Add, add, rs_simd_f128_complex_add, 4);
impl_arithmetic_ops!(Complex128, Sub, sub, rs_simd_f128_complex_sub, 4);
impl_arithmetic_ops!(Complex128, Mul, mul, rs_simd_f128_complex_mul, 4);
impl_arithmetic_ops!(Complex128, Div, div, rs_simd_f128_complex_div, 4);

impl_assign_ops!(Complex128, AddAssign, add_assign, add);
impl_assign_ops!(Complex128, SubAssign, sub_assign, sub);
impl_assign_ops!(Complex128, MulAssign, mul_assign, mul);
impl_assign_ops!(Complex128, DivAssign, div_assign, div);

impl Neg for Float128 {
    type Output = Self;
    fn neg(self) -> Self {
        let mut out = [0.0; 2];
        unsafe {
            rs_simd_f128_neg(self.data.as_ptr(), out.as_mut_ptr());
        }
        Float128 { data: out }
    }
}
impl Neg for &Float128 {
    type Output = Float128;
    fn neg(self) -> Float128 {
        -(*self)
    }
}

impl Neg for Complex128 {
    type Output = Self;
    fn neg(self) -> Self {
        Self::new_f128(-self.real(), -self.imag())
    }
}
impl Neg for &Complex128 {
    type Output = Complex128;
    fn neg(self) -> Complex128 {
        -(*self)
    }
}

// =========================================================================
// native type interoperability (f64)
// =========================================================================
impl From<f64> for Float128 {
    fn from(val: f64) -> Self {
        Float128::new(val)
    }
}

impl From<Float128> for f64 {
    fn from(val: Float128) -> f64 {
        val.extract().0
    }
}

macro_rules! impl_f64_arithmetic {
    ($trait:ident, $method:ident) => {
        impl $trait<f64> for Float128 {
            type Output = Float128;
            fn $method(self, other: f64) -> Float128 {
                self.$method(Float128::new(other))
            }
        }
        impl $trait<f64> for &Float128 {
            type Output = Float128;
            fn $method(self, other: f64) -> Float128 {
                (*self).$method(Float128::new(other))
            }
        }
        impl $trait<Float128> for f64 {
            type Output = Float128;
            fn $method(self, other: Float128) -> Float128 {
                Float128::new(self).$method(other)
            }
        }
        impl $trait<&Float128> for f64 {
            type Output = Float128;
            fn $method(self, other: &Float128) -> Float128 {
                Float128::new(self).$method(*other)
            }
        }
    };
}

impl_f64_arithmetic!(Add, add);
impl_f64_arithmetic!(Sub, sub);
impl_f64_arithmetic!(Mul, mul);
impl_f64_arithmetic!(Div, div);

// =========================================================================
// comparison & formatting
// =========================================================================
impl PartialEq for Float128 {
    fn eq(&self, other: &Self) -> bool {
        if self.data[0].is_nan() || other.data[0].is_nan() {
            return false;
        }
        unsafe { rs_simd_f128_cmp(self.data.as_ptr(), other.data.as_ptr()) == 0 }
    }
}

impl PartialOrd for Float128 {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        if self.data[0].is_nan() || other.data[0].is_nan() {
            return None;
        }
        unsafe {
            let cmp = rs_simd_f128_cmp(self.data.as_ptr(), other.data.as_ptr());
            if cmp < 0 {
                Some(Ordering::Less)
            } else if cmp > 0 {
                Some(Ordering::Greater)
            } else {
                Some(Ordering::Equal)
            }
        }
    }
}

impl Default for Float128 {
    fn default() -> Self {
        Float128::new(0.0)
    }
}
impl Default for Complex128 {
    fn default() -> Self {
        Complex128::new()
    }
}

impl std::str::FromStr for Float128 {
    type Err = &'static str;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let c_str = std::ffi::CString::new(s).map_err(|_| "invalid string")?;
        let mut out = [0.0; 2];
        unsafe {
            rs_simd_f128_from_string(c_str.as_ptr(), out.as_mut_ptr());
        }
        Ok(Float128 { data: out })
    }
}

impl std::fmt::Display for Float128 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut buf = [0u8; 128];
        unsafe {
            rs_simd_f128_to_string(self.data.as_ptr(), buf.as_mut_ptr() as *mut std::os::raw::c_char, buf.len());
        }
        let end = buf.iter().position(|&x| x == 0).unwrap_or(buf.len());
        if let Ok(s) = std::str::from_utf8(&buf[..end]) {
            write!(f, "{}", s)
        } else {
            let (hi, lo) = self.extract();
            write!(f, "Float128({:.16} + {:.16})", hi, lo)
        }
    }
}
impl std::fmt::Display for Complex128 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} + {}i", self.real(), self.imag())
    }
}
