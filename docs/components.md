# Library Components

All headers are `static inline` / header-only. Include the relevant header in your project — no separate compilation unit is needed (beyond the one file that defines `SIMD_F128_IMPLEMENTATION`).

---

## simd_f128.h (Core)

The central engine. Implements the `simd_f128` type and all fundamental arithmetic operations.

**Key properties:**

- **~106-bit mantissa** — roughly 31-32 decimal digits of precision.
- **Zero heap allocation** — all operations execute directly in CPU registers, suitable for tight inner loops.
- **Automatic SIMD dispatch** — selects AVX2/SSE2 (`__m128d`) on Intel/AMD, NEON (`float64x2_t`) on ARM64/Apple Silicon, WASM-SIMD (`v128_t`) on the web, or falls back to scalar C99.
- **Branch-free fast paths** — minimal branching (restricted to `Inf`/`NaN` guards) ensures consistent execution time and avoids pipeline stalls in the hot path.
- **Strict IEEE 754 foundation** — built on standard `double`, fully compatible with existing hardware.

```c
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.h>

int main() {
    simd_f128 a = simd_f128_from_double(1.234567890123456789);
    simd_f128 b = simd_f128_from_double(2.0);

    simd_f128 sum  = simd_f128_add(a, b);
    simd_f128 diff = simd_f128_sub(a, b);
    simd_f128 prod = simd_f128_mul(a, b);
    simd_f128 quot = simd_f128_div(a, b);
    simd_f128 root = simd_f128_sqrt(a);

    return 0;
}
```

---

## simd_f128_consts.h

Pre-computed, high-precision mathematical constants stored as Double-Double pairs. Each constant captures the full ~106-bit mantissa, avoiding the precision loss inherent in standard 64-bit initialisers.

```c
#include <simd_f128.h>
#include <simd_f128_consts.h>

int main() {
    simd_f128 pi     = SIMD_F128_PI;    // 3.14159265358979323846...
    simd_f128 e      = SIMD_F128_E;     // 2.71828182845904523536...
    simd_f128 sqrt_2 = SIMD_F128_SQRT2; // 1.41421356237309504880...
    simd_f128 ln2    = SIMD_F128_LN2;   // 0.69314718055994530941...

    return 0;
}
```

---

## simd_f128_io.h

Handles conversion between the internal Double-Double representation and human-readable decimal strings. Standard `printf` formatting cannot faithfully render 128-bit values; this header uses an iterative high-precision extraction algorithm to produce up to 32 correct decimal places.

```c
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.h>
#include <simd_f128_io.h>

int main() {
    // parsing from string maintains the full 31-digit precision
    simd_f128 val = simd_f128_from_string("3.1415926535897932384626433832795");

    // direct console output
    simd_f128_print(val);

    // string conversion for logging or ui
    char buffer[128];
    simd_f128_to_string(buffer, sizeof(buffer), val);

    return 0;
}
```

---

## simd_f128_math.h

Advanced mathematical functions built on top of the core Double-Double primitives. All functions are `static inline` and require no additional compilation unit.

**Algorithms used:**

- **`exp`** — range reduction to $N=16$ intervals followed by a high-degree Chebyshev minimax polynomial, then exact scaling via `ldexp` and a 16-entry lookup table. Handles overflow (> 709.78) and underflow explicitly.
- **`log`** — seeds from the standard `double` `log()`, then refines with 1-2 iterations of Halley's method (2 iterations for subnormal inputs), which is mathematically sufficient to recover all 31-32 digits due to cubic convergence.
- **`pow`** — computed as `exp(exponent * log(base))`. Fully protected against integer overflow during exponent parity checks. Supports base-zero inputs and propagates `NaN` according to IEEE-754.
- **`sin`** — range-reduces to quadrant ($[-\pi/4, \pi/4]$) then evaluates a highly-tuned Chebyshev minimax polynomial.
- **`cos`** — range-reduces to quadrant ($[-\pi/4, \pi/4]$) then evaluates a highly-tuned Chebyshev minimax polynomial.
- **`sincos`** — computes both sine and cosine simultaneously, saving redundant range reduction and polynomial evaluation steps.
- **`sinh` / `tanh`** — evaluate via Taylor series near zero to prevent catastrophic cancellation, falling back to exponential formulations for larger inputs.
- **`atan` / `atan2` / `asin` / `acos`** — inverse trigonometric functions with proper domain guards.
- **`floor` / `ceil` / `trunc` / `round` / `fmod`** — rounding and remainder functions operating on the full Double-Double pair.
- **`log2` / `log10` / `cbrt`** — derived from the core `log` and `pow` implementations.

```c
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.h>
#include <simd_f128_consts.h>
#include <simd_f128_math.h>

int main() {
    simd_f128 x = SIMD_F128_PI;

    // e^π
    simd_f128 epi = simd_f128_exp(x);

    // ln(e) == 1
    simd_f128 one = simd_f128_log(SIMD_F128_E);

    // 2^10 == 1024
    simd_f128 base = simd_f128_from_double(2.0);
    simd_f128 exp  = simd_f128_from_double(10.0);
    simd_f128 pw   = simd_f128_pow(base, exp);

    // sin(π/6) == 0.5
    simd_f128 half_pi = simd_f128_mul(x, simd_f128_from_double(1.0 / 6.0));
    simd_f128 s       = simd_f128_sin(half_pi);

    // cos(0) == 1
    simd_f128 c = simd_f128_cos(simd_f128_from_double(0.0));

    return 0;
}
```

> **Note:** `sin` and `cos` use a simplified range reduction suitable for moderate arguments. For very large inputs (|x| > ~10^15), consider applying Payne-Hanek argument reduction externally before calling these functions.

---

## simd_f128_utils.h

Comparison operators and utility functions. All are `static inline` and work with any SIMD backend.

The foundation is `simd_f128_cmp`, which compares the `hi` components first and only falls through to the `lo` components when `hi` values are identical — matching the canonical Double-Double ordering rule.

```c
#include <simd_f128.h>
#include <simd_f128_utils.h>

int main() {
    simd_f128 a = simd_f128_from_double(1.0);
    simd_f128 b = simd_f128_from_double(2.0);

    // comparisons
    int lt = simd_f128_lt(a, b);  // 1
    int eq = simd_f128_eq(a, b);  // 0
    int ge = simd_f128_ge(b, a);  // 1

    // utility
    simd_f128 neg = simd_f128_from_double(-3.14);
    simd_f128 abs_val = simd_f128_abs(neg);       // 3.14...
    simd_f128 lo      = simd_f128_min(a, b);      // 1.0
    simd_f128 hi      = simd_f128_max(a, b);      // 2.0

    return 0;
}
```

---

## simd_f128_matrix.h

Fixed-size matrix and vector types for 2D, 3D, and 4D linear algebra at full 128-bit precision.

| Type | Description |
|---|---|
| `simd_f128_mat2` | 2×2 matrix |
| `simd_f128_mat3` | 3×3 matrix |
| `simd_f128_mat4` | 4×4 matrix |
| `simd_f128_vec2` | 2-component vector (`x`, `y`) |
| `simd_f128_vec3` | 3-component vector (`x`, `y`, `z`) |
| `simd_f128_vec4` | 4-component vector (`x`, `y`, `z`, `w`) |

---

## simd_f128_random.h

A high-quality PRNG (xoshiro256\*\*) that produces uniform `simd_f128` values in `[0, 1)` with full Double-Double precision. State is caller-managed — no heap allocation.

```c
#include <simd_f128.h>
#include <simd_f128_random.h>

int main() {
    simd_f128_prng rng;
    simd_f128_prng_seed(&rng, 42);

    simd_f128 r = simd_f128_prng_uniform(&rng); // [0, 1)

    return 0;
}
```

---

## simd_f128.hpp (C++ only)

A modern C++ wrapper that makes `simd_f128` feel like a native arithmetic type. Include this single header in C++ projects — it pulls in all other headers automatically.

**Features:**

- `f128::float128` class with full operator overloading (`+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`).
- Full interoperability with `std::complex<double>` via `f128::complex128`.
- Seamless integration with the **Eigen** matrix library via `simd_f128_eigen.hpp`.
- All six comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`).
- Unary negation (`-x`).
- `std::ostream` integration (`std::cout << val`).
- Free functions mirroring `<cmath>`: `f128::exp`, `f128::log`, `f128::pow`, `f128::sin`, `f128::cos`, `f128::sqrt`, `f128::abs`.
- Predefined constants: `f128::pi`, `f128::e`, `f128::sqrt2`, `f128::ln2`.

```cpp
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.hpp>
#include <iostream>

int main() {
    f128::float128 a(1.5);
    f128::float128 b(2.5);

    // natural arithmetic
    f128::float128 sum  = a + b;
    f128::float128 prod = a * b;

    // math functions
    f128::float128 root = f128::sqrt(a);
    f128::float128 s    = f128::sin(f128::pi);

    // stream output
    std::cout << "a + b = " << sum  << "\n";
    std::cout << "a * b = " << prod << "\n";
    std::cout << "sqrt(a) = " << root << "\n";

    // comparisons
    if (a < b) {
        std::cout << "a is smaller\n";
    }

    return 0;
}
```

The `float128` class stores a `simd_f128 data` member publicly, so it can be passed directly to any C API function when needed:

```cpp
f128::float128 val(3.14);
simd_f128_print(val.data);  // call c api directly
```
