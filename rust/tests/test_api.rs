/* test_api.rs
 *
 * unit tests for simd-f128.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

use simd_f128::{Float128, Complex128};

#[test]
fn test_float128_f64_ops() {
    let a = Float128::new(1.0);
    let b = a + 2.0;
    assert_eq!(b.extract().0, 3.0);

    let c = 2.5 + b;
    assert_eq!(c.extract().0, 5.5);

    let d = &a + &b;
    assert_eq!(d.extract().0, 4.0);
}

#[test]
fn test_complex128_math() {
    let a = Complex128::new_f64(1.0, 2.0);
    let b = Complex128::new_f64(3.0, 4.0);
    let c = a + b;
    assert_eq!(c.real().extract().0, 4.0);
    assert_eq!(c.imag().extract().0, 6.0);

    let sqr = c.abs_sqr();
    assert_eq!(sqr.extract().0, 52.0); // 4^2 + 6^2

    // transcendental
    let s = a.sin();
    // check it doesn't panic
    let _ = s.real().extract().0;
}

#[test]
fn test_ieee754_signed_zero() {
    let pos_zero = Float128::new(0.0);
    let neg_zero = Float128::new(-0.0);
    let one = Float128::new(1.0);

    let neg_inf = one / neg_zero;
    assert!(neg_inf.extract().0.is_infinite());
    assert!(neg_inf.extract().0 < 0.0);

    let pos_inf = one / pos_zero;
    assert!(pos_inf.extract().0.is_infinite());
    assert!(pos_inf.extract().0 > 0.0);

    let sum_zero = neg_zero + pos_zero;
    assert!(!sum_zero.extract().0.is_sign_negative());
}

#[test]
fn test_ieee754_nan_propagation() {
    let nan_val = Float128::new(std::f64::NAN);
    let norm = Float128::new(42.0);

    let r1 = nan_val + norm;
    let r2 = nan_val * norm;

    assert!(r1.extract().0.is_nan());
    assert!(r2.extract().0.is_nan());
}

#[test]
fn test_ieee754_infinity_arithmetic() {
    let inf = Float128::new(std::f64::INFINITY);
    let norm = Float128::new(42.0);
    let zero = Float128::new(0.0);

    let r1 = inf + norm;
    assert!(r1.extract().0.is_infinite());

    let r2 = inf - inf;
    assert!(r2.extract().0.is_nan());

    let r3 = inf * zero;
    assert!(r3.extract().0.is_nan());
}

#[test]
fn test_ieee754_comparisons() {
    let nan_val = Float128::new(std::f64::NAN);
    let norm = Float128::new(42.0);

    assert!(nan_val != norm);
    // IEEE-754: NaN != NaN
    assert!(nan_val != nan_val);
    assert!(!(nan_val == nan_val));
    assert!(!(nan_val > norm));
    assert!(!(nan_val < norm));
}
