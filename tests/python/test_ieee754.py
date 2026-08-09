# test_ieee754.py
#
# python bindings ieee-754 conformance tests.
# validates nan/infinity handling from python level down to c-core.
#
# updated 2026-08-09
# spdx-license-identifier: mit
# copyright (c) 2026 jirawat siripuk

import math
import sys
import pytest

try:
    import simd_f128
except ImportError as e:
    print(f"Failed to import simd_f128: {e}")
    sys.exit(1)

def test_ieee754_signed_zero() -> None:
    # [TEST CASE] signed zero handling
    pos_zero = simd_f128.Float128(0.0)
    neg_zero = simd_f128.Float128(-0.0)
    one = simd_f128.Float128(1.0)
    
    # 1.0 / -0.0 = -inf
    neg_inf = one / neg_zero
    assert "inf" in neg_inf.to_string().lower() or "infinity" in neg_inf.to_string().lower()
    assert "-" in neg_inf.to_string()
    
    # 1.0 / +0.0 = +inf
    pos_inf = one / pos_zero
    assert "inf" in pos_inf.to_string().lower() or "infinity" in pos_inf.to_string().lower()
    assert "-" not in pos_inf.to_string()

def test_ieee754_nan_propagation() -> None:
    # [TEST CASE] nan propagation
    nan_val = simd_f128.Float128(math.nan)
    norm = simd_f128.Float128(42.0)
    
    r1 = nan_val + norm
    r2 = nan_val * norm
    
    assert "nan" in r1.to_string().lower()
    assert "nan" in r2.to_string().lower()

def test_ieee754_infinity_arithmetic() -> None:
    # [TEST CASE] infinity arithmetic
    inf = simd_f128.Float128(math.inf)
    norm = simd_f128.Float128(42.0)
    zero = simd_f128.Float128(0.0)
    
    # inf + norm = inf
    r1 = inf + norm
    assert "inf" in r1.to_string().lower()
    
    # inf - inf = nan
    r2 = inf - inf
    assert "nan" in r2.to_string().lower()
    
    # inf * 0 = nan
    r3 = inf * zero
    assert "nan" in r3.to_string().lower()
