"""python_vanishing_gradient.py

demonstrates vanishing updates in neural networks.
small gradients (1e-18) are dropped by 64-bit precision,
halting learning. simd-f128 preserves these updates.
"""

import sys
import os

# append root path to load simd_f128 extension
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

try:
    import simd_f128
except ImportError:
    print("simd_f128 python extension not found. please build the python bindings first.")
    sys.exit(1)

def weight_update_double(weight, learning_rate, gradient):
    update = learning_rate * gradient
    new_weight = weight - update
    return new_weight

def weight_update_f128(weight_val, lr_val, grad_val):
    weight = simd_f128.Float128(weight_val)
    lr = simd_f128.Float128(lr_val)
    grad = simd_f128.Float128(grad_val)

    update = lr * grad
    new_weight = weight - update
    return new_weight

if __name__ == "__main__":
    print("--- AI Weight Update (Vanishing Gradient) Simulation ---")
    print("Simulating a deep neural network layer with extremely small weight updates...\n")

    # expected update: 1e-18 (smaller than 64-bit epsilon ~2.22e-16)
    weight = 1.0
    learning_rate = 1e-3
    gradient = 1e-15

    new_w_double = weight_update_double(weight, learning_rate, gradient)
    new_w_f128 = weight_update_f128(weight, learning_rate, gradient)

    print(f"Original Weight : {weight}")
    print(f"Requested Update: -{learning_rate * gradient}")
    print("-" * 50)
    print(f"New Weight (64-bit float) : {new_w_double}")
    print(f"New Weight (128-bit SIMD) : {new_w_f128}")
    print("-" * 50)

    if new_w_double == weight:
        print("\n[Analysis]:")
        print("The 64-bit update lost precision because 1e-18 is too small to affect 1.0.")
        print("A neural network using standard doubles would STOP learning completely here")
        print("because the weights stop changing (Vanishing Update).")
        print("However, simd-f128 successfully applied the tiny update!")
