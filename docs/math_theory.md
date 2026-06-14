# Double-Double Arithmetic

simd-f128 represents a value $x$ as the unevaluated sum of two IEEE 754 doubles:

$$x = x_{hi} + x_{lo}, \quad |x_{lo}| \leq \frac{1}{2} \, \text{ulp}(x_{hi})$$

This non-overlapping constraint provides ~106 bits of mantissa — approximately double the precision of a single `double`.

**Implementation basis:**

- **Addition: TwoSum (Knuth)** — An error-free transformation (EFT) for addition that captures the exact rounding residual.
- **Multiplication: TwoProd (Dekker)** — Exploits hardware FMA (Fused Multiply-Add) where available. On platforms lacking FMA, it seamlessly falls back to **Veltkamp's Split** to divide 53-bit mantissas into 26-bit halves, calculating the exact error product natively without precision loss.
- **Division: Newton-Raphson Iteration** — Approximates the reciprocal $1/b_{hi}$ and refines it quadratically. Includes rigorous guards against `NaN` propagation during division-by-zero scenarios.
- **Square Root: Newton-Raphson with Residual Correction** — Uses the hardware `sqrt` instruction to generate a perfect 53-bit initial guess, followed by a Newton-Raphson iteration with residual correction to accurately recover the full ~106-bit mantissa.
- **Normalisation** — Every arithmetic operation rigidly re-establishes the non-overlapping property before returning.

No memory allocation is required. The entire number lives in two registers.

**Known limitations:**

- Numerical range is identical to IEEE 754 `double` (~1.8 × 10^308). The library extends mantissa precision only; exponent range is unchanged.
- `NaN` and `Infinity` propagate through standard `double` rules.
- `sin` and `cos` use simplified range reduction. For large arguments (|x| ≫ 2π), apply Payne-Hanek reduction externally before calling.
- `pow` does not support negative bases; use `simd_f128_mul` + `simd_f128_exp` for integer powers of negative numbers.
- On ARMv7, FMA requires VFPv4 hardware (Cortex-A7, A15, A17, A53+) and the `-mfpu=neon-vfpv4` flag.

## Transcendental & Trigonometric Functions

Calculating advanced math functions (like `exp`, `log`, `sin`, `cos`) with 106-bit precision cannot rely on standard C math libraries. `simd-f128` implements custom high-precision approximations:

- **Exponential (`exp`)**: Scales the input by $\log_2(e)$ to reduce the range to $[0, 1)$, then evaluates a high-precision minimax polynomial approximation. The integer part is handled by directly scaling the floating-point exponent, while the fractional part is reconstructed with full Double-Double precision.
- **Logarithm (`log`)**: Uses range reduction to extract the IEEE-754 mantissa and exponent. The mantissa is mapped close to $1.0$, and the logarithm is calculated using a high-degree Taylor/Minimax polynomial expansion. The final result combines the integer exponent term ($n \times \ln(2)$) with the polynomial output.
- **Trigonometric (`sin`, `cos`)**: Employs range reduction to map the input into the interval $[-\pi/4, \pi/4]$. The core computation evaluates carefully tuned 12th-degree **Chebyshev Polynomials**, guaranteeing near-perfect precision across the reduced range without using a Taylor series, which converges too slowly for 106-bit accuracy.
