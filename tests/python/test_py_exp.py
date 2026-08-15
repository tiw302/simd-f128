# test_py_exp.py
#
# python math exponentiation tests.
#
# updated 2026-08-13
# spdx-license-identifier: mit
# copyright (c) 2026 jirawat siripuk

import math

def test_math_exp_thresholds() -> None:
    # math exp thresholds
    result1: float = math.exp(709.5)
    result2: float = math.exp(709.1)

    assert result1 > 0.0
    assert result2 > 0.0
