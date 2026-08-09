# test_py_exp.py
#
# tests for standard python math exponentiation behavior.
# validates bounds and threshold behavior for exp().
#
# updated 2026-08-09
# spdx-license-identifier: mit
# copyright (c) 2026 jirawat siripuk

import math

def test_math_exp_thresholds() -> None:
    # [TEST CASE] math exp thresholds
    # validates standard math.exp behavior near the overflow threshold.
    result1: float = math.exp(709.5)
    result2: float = math.exp(709.1)
    
    assert result1 > 0.0
    assert result2 > 0.0
