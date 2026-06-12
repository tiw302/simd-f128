import os
import sys

# attempt to import the built extension
try:
    import simd_f128
except ImportError as e:
    print(f"Failed to import simd_f128: {e}")
    sys.exit(1)

def test_python_bindings():
    errors = 0
    
    print("[1] Testing Float128...")
    a = simd_f128.Float128(1.5)
    b = simd_f128.Float128("2.5")
    
    c = a + b
    if not c.to_string().startswith("4.0"):
        print(f"Error: Float128 Add failed. Got {c.to_string()}")
        errors += 1

    d = a * b
    if not d.to_string().startswith("3.75"):
        print(f"Error: Float128 Mul failed. Got {d.to_string()}")
        errors += 1

    print("[2] Testing Complex128...")
    c1 = simd_f128.Complex128(1.0, 2.0)
    c2 = simd_f128.Complex128(3.0, 4.0)
    
    c_mul = c1 * c2 # (1+2i)*(3+4i) = -5+10i
    if not c_mul.real().to_string().startswith("-5.0"):
        print(f"Error: Complex Mul real failed. Got {c_mul.real().to_string()}")
        errors += 1
        
    if not c_mul.imag().to_string().startswith("10.0"):
        print(f"Error: Complex Mul imag failed. Got {c_mul.imag().to_string()}")
        errors += 1

    print("[3] Testing Math Constants...")
    pi = simd_f128.pi
    if not pi.to_string().startswith("3.14159265"):
        print(f"Error: PI constant failed. Got {pi.to_string()}")
        errors += 1

    print("[4] Testing New Math Functions...")
    # test tan, sinh, cosh, tanh
    zero = simd_f128.Float128(0.0)
    tan_zero = simd_f128.tan(zero)
    if not tan_zero.to_string().startswith("0.0"):
        print(f"Error: Float128 tan(0) failed. Got {tan_zero.to_string()}")
        errors += 1

    one = simd_f128.Float128(1.0)
    sinh_one = simd_f128.sinh(one)
    cosh_one = simd_f128.cosh(one)
    tanh_one = simd_f128.tanh(one)
    if not sinh_one.to_string().startswith("1.17520119"):
        print(f"Error: Float128 sinh(1) failed. Got {sinh_one.to_string()}")
        errors += 1

    comp_z = simd_f128.Complex128(1.0, 1.0)
    comp_sinh = simd_f128.sinh(comp_z)
    if not comp_sinh.real().to_string().startswith("0.634963"):
        print(f"Error: Complex128 sinh(1+i) failed. Got {comp_sinh.real().to_string()}")
        errors += 1

    if errors == 0:
        print("All Python Binding Tests Passed!")
        sys.exit(0)
    else:
        print(f"{errors} tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    test_python_bindings()
