/*
 * lib.rs -- high-performance 128-bit (double-double) arithmetic for rust.
 * project url: https://github.com/tiw302/simd-f128
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 * rust bindings:
 * --------------
 * this file provides safe rust abstractions over the underlying c-core 
 * functions via ffi. it implements standard rust traits (std::ops) so 
 * that float128 feels like a native rust primitive with zero overhead.
 * license:
 * --------
 * mit license
 * copyright (c) 2026 jirawat siripuk
 * */

use std::ops::{Add, Sub, Mul, Div, AddAssign, SubAssign, MulAssign, DivAssign, Neg};
use std::cmp::Ordering;

// the core 128-bit floating point type.
// we force a strict c representation and 16-byte memory alignment (align(16)).
// this ensures that when rust passes pointers to the c backend, the memory 
// perfectly aligns with avx2 and wasm simd128 vector register requirements.
#[repr(C, align(16))]
#[derive(Copy, Clone, Debug)]
pub struct Float128 {
    // must match the c struct layout exactly (hi, lo)
    data: [f64; 2],
}

// ffi bindings to the c-core backend.
// we use pointer passing (*const f64, *mut f64) instead of passing structs by value.
// this completely sidesteps c abi calling convention bugs across different operating systems,
// ensuring the parameters are cleanly mapped to memory registers without unexpected stack copies.
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
}

// safe rust abstractions over the unsafe ffi boundary.
// every method here allocates the result array (`let mut out = [0.0; 2]`) 
// purely on the rust stack. this guarantees zero heap allocations while 
// remaining perfectly thread-safe.
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
    // all mathematical operations defer to the heavily optimized c-core.
    // the underlying implementations utilize 128-bit minimax polynomials, 
    // newton-raphson iterations, and hardware fused multiply-add (fma) 
    // when compiled with avx2 or neon backends. precision is strictly bounded
    // to ~31 decimal digits across the entire domain space.
    
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

    // =========================================================================
    // exact pre-computed mathematical constants
    // =========================================================================
    // these constants are injected directly from the c-core's read-only memory segment.
    // they are pre-calculated to exact 128-bit double-double precision limits,
    // ensuring no precision is lost during initialization.

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

// operator overloading for native rust ergonomics.
// by implementing standard std::ops traits, float128 can be used exactly like a native f64.
// rust's llvm backend is smart enough to inline these wrapper traits away, meaning 
// the abstraction overhead drops to absolute zero at runtime.
impl Add for Float128 {
    type Output = Self;
    fn add(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_add(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }
}

impl Sub for Float128 {
    type Output = Self;
    fn sub(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_sub(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }
}

impl Mul for Float128 {
    type Output = Self;
    fn mul(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_mul(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }
}

impl Div for Float128 {
    type Output = Self;
    fn div(self, other: Self) -> Self {
        let mut out = [0.0; 2];
        unsafe { rs_simd_f128_div(self.data.as_ptr(), other.data.as_ptr(), out.as_mut_ptr()); }
        Float128 { data: out }
    }
}

impl AddAssign for Float128 {
    fn add_assign(&mut self, other: Self) {
        *self = *self + other;
    }
}

impl SubAssign for Float128 {
    fn sub_assign(&mut self, other: Self) {
        *self = *self - other;
    }
}

impl MulAssign for Float128 {
    fn mul_assign(&mut self, other: Self) {
        *self = *self * other;
    }
}

impl DivAssign for Float128 {
    fn div_assign(&mut self, other: Self) {
        *self = *self / other;
    }
}

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

// float128 does not implement eq or ord because nan comparison properties
// make total ordering mathematically impossible under strict ieee-754 semantics.
// instead, partial_eq and partial_ord are implemented to correctly handle
// comparisons (returning false or none when nan is involved).
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

// default initialization.
// returns a new Float128 initialized to 0.0.
impl Default for Float128 {
    fn default() -> Self {
        Float128::new(0.0)
    }
}

// string parsing via ffi.
// we safely allocate a null-terminated c_string in rust memory space first.
// this prevents the c-core from reading uninitialized rust memory, preventing segfaults.
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

// deterministic formatting engine.
// we pass a 128-byte stack-allocated buffer to the c-core to format the string.
// this is extremely fast and avoids rust string allocation overhead until the final utf-8 slice.
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_default() {
        let a = Float128::default();
        let (hi, lo) = a.extract();
        assert_eq!(hi, 0.0);
        assert_eq!(lo, 0.0);
    }

    #[test]
    fn test_addition() {
        let a = Float128::new(1.0);
        let b = Float128::new(1e-17);
        let c = a + b;
        let (hi, lo) = c.extract();
        assert_eq!(hi, 1.0);
        assert_eq!(lo, 1e-17);
    }

    #[test]
    fn test_neg() {
        let a = Float128::new(1.0);
        let b = -a;
        assert_eq!(b.extract().0, -1.0);
    }

    #[test]
    fn test_from_str() {
        use std::str::FromStr;
        let pi = Float128::from_str("3.14159265358979323846264338327950").unwrap();
        assert!(pi.extract().0 > 3.14);
    }

    #[test]
    fn test_display() {
        let pi = Float128::pi();
        let s = format!("{}", pi);
        assert!(s.starts_with("3.1415926535897932"));
    }

    #[test]
    fn test_advanced_math() {
        let two = Float128::new(2.0);
        let three = Float128::new(3.0);
        let pow_res = two.pow(three);
        let (hi, _) = pow_res.extract();
        assert_eq!(hi, 8.0);

        let neg_four_point_seven = Float128::new(-4.7);
        assert_eq!(neg_four_point_seven.abs().extract().0, 4.7);
        assert_eq!(neg_four_point_seven.floor().extract().0, -5.0);
        assert_eq!(neg_four_point_seven.ceil().extract().0, -4.0);
        assert_eq!(neg_four_point_seven.round().extract().0, -5.0);
        assert_eq!(neg_four_point_seven.trunc().extract().0, -4.0);

        let ten_point_five = Float128::new(10.5);
        let three_f128 = Float128::new(3.0);
        assert_eq!(ten_point_five.fmod(three_f128).extract().0, 1.5);

        let one = Float128::new(1.0);
        let asin_res = one.asin();
        let pi_over_2 = Float128::pi() * Float128::new(0.5);
        assert!((asin_res - pi_over_2).abs().extract().0 < 1e-12);
    }

    #[test]
    fn test_hyperbolic_and_trig() {
        // test tan
        let zero = Float128::new(0.0);
        let tan_zero = zero.tan();
        assert!((tan_zero.extract().0).abs() < 1e-15);

        // tan(pi/4) should be approximately 1.0
        let pi_over_4 = Float128::pi() * Float128::new(0.25);
        let tan_pi_over_4 = pi_over_4.tan();
        assert!((tan_pi_over_4.extract().0 - 1.0).abs() < 1e-12);

        // test sinh, cosh, tanh
        let one = Float128::new(1.0);
        let sinh_one = one.sinh();
        let cosh_one = one.cosh();
        let tanh_one = one.tanh();

        // sinh(1) = (e - 1/e) / 2
        // cosh(1) = (e + 1/e) / 2
        // tanh(1) = sinh(1) / cosh(1)
        let e = Float128::e();
        let inv_e = Float128::new(1.0) / e;
        let expected_sinh = (e - inv_e) * Float128::new(0.5);
        let expected_cosh = (e + inv_e) * Float128::new(0.5);
        let expected_tanh = expected_sinh / expected_cosh;

        assert!((sinh_one - expected_sinh).abs().extract().0 < 1e-15);
        assert!((cosh_one - expected_cosh).abs().extract().0 < 1e-15);
        assert!((tanh_one - expected_tanh).abs().extract().0 < 1e-15);
    }
}
