# test_python.py
#
# python bindings unit tests.
# validates arithmetic and math functions via pytest.
#
# updated 2026-08-09
# spdx-license-identifier: mit
# copyright (c) 2026 jirawat siripuk

import sys
import pytest

try:
    import simd_f128
except ImportError as e:
    print(f"Failed to import simd_f128: {e}")
    sys.exit(1)


def test_float128_addition() -> None:
    # [TEST CASE] float128 addition
    # verifies addition of float128 values.
    val1 = simd_f128.Float128(1.5)
    val2 = simd_f128.Float128("2.5")
    
    result = val1 + val2
    assert result.to_string().startswith("4.0"), f"Expected 4.0, got {result.to_string()}"


def test_float128_multiplication() -> None:
    # [TEST CASE] float128 multiplication
    # verifies multiplication of float128 values.
    val1 = simd_f128.Float128(1.5)
    val2 = simd_f128.Float128("2.5")
    
    result = val1 * val2
    assert result.to_string().startswith("3.75"), f"Expected 3.75, got {result.to_string()}"


def test_complex128_multiplication() -> None:
    # [TEST CASE] complex128 multiplication
    # verifies multiplication of complex128 values.
    comp1 = simd_f128.Complex128(1.0, 2.0)
    comp2 = simd_f128.Complex128(3.0, 4.0)

    # (1+2i)*(3+4i) = -5+10i
    result = comp1 * comp2
    assert result.real().to_string().startswith("-5.0"), f"Expected real -5.0, got {result.real().to_string()}"
    assert result.imag().to_string().startswith("10.0"), f"Expected imag 10.0, got {result.imag().to_string()}"


def test_math_constants() -> None:
    # [TEST CASE] math constants
    # verifies mathematical constants provided by the bindings.
    pi_val = simd_f128.pi
    assert pi_val.to_string().startswith("3.14159265"), f"Expected PI ~ 3.14159265, got {pi_val.to_string()}"


def test_math_functions_float128() -> None:
    # [TEST CASE] float128 math functions
    # verifies float128 math functions like tan, sinh, cosh, tanh.
    zero = simd_f128.Float128(0.0)
    tan_zero = simd_f128.tan(zero)
    assert tan_zero.to_string().startswith("0.0"), f"Expected 0.0, got {tan_zero.to_string()}"

    one = simd_f128.Float128(1.0)
    sinh_one = simd_f128.sinh(one)
    
    assert sinh_one.to_string().startswith("1.17520119"), f"Expected ~1.17520119, got {sinh_one.to_string()}"


def test_math_functions_complex128() -> None:
    # [TEST CASE] complex128 math functions
    # verifies complex128 math functions like sinh.
    comp_z = simd_f128.Complex128(1.0, 1.0)
    comp_sinh = simd_f128.sinh(comp_z)
    
    assert comp_sinh.real().to_string().startswith("0.634963"), f"Expected ~0.634963, got {comp_sinh.real().to_string()}"
