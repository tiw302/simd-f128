use std::ops::{Add, Sub, Mul, Div, AddAssign, SubAssign, MulAssign, DivAssign};
use std::cmp::Ordering;

#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub struct Float128 {
    // must match the c struct layout exactly (hi, lo)
    data: [f64; 2],
}

extern "C" {
    fn rs_simd_f128_add(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_sub(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_mul(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_div(a: *const f64, b: *const f64, out: *mut f64);
    fn rs_simd_f128_sqrt(a: *const f64, out: *mut f64);
    fn rs_simd_f128_exp(a: *const f64, out: *mut f64);
    fn rs_simd_f128_log(a: *const f64, out: *mut f64);
    fn rs_simd_f128_sin(a: *const f64, out: *mut f64);
    fn rs_simd_f128_cos(a: *const f64, out: *mut f64);
    fn rs_simd_f128_from_double(d: f64, out: *mut f64);
    fn rs_simd_f128_cmp(a: *const f64, b: *const f64) -> std::os::raw::c_int;
}

impl Float128 {
    pub fn new(val: f64) -> Self {
        let mut out = [0.0; 2];
        // safely pass memory boundary to c function
        unsafe {
            rs_simd_f128_from_double(val, out.as_mut_ptr());
        }
        Float128 { data: out }
    }

    pub fn extract(&self) -> (f64, f64) {
        (self.data[0], self.data[1])
    }

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
}

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

impl PartialEq for Float128 {
    fn eq(&self, other: &Self) -> bool {
        unsafe { rs_simd_f128_cmp(self.data.as_ptr(), other.data.as_ptr()) == 0 }
    }
}

impl PartialOrd for Float128 {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
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

impl std::fmt::Display for Float128 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let (hi, lo) = self.extract();
        // Fallback simple formatting since we don't have to_string mapped yet
        write!(f, "Float128({:.16} + {:.16})", hi, lo)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_addition() {
        let a = Float128::new(1.0);
        let b = Float128::new(1e-17);
        let c = a + b;
        let (hi, lo) = c.extract();
        println!("a = {:?}", a);
        println!("b = {:?}", b);
        println!("c = {:?}", c);
        println!("hi = {}, lo = {}", hi, lo);
        assert_eq!(hi, 1.0);
        assert_eq!(lo, 1e-17);
    }
}
