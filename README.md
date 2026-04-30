<p align="center">
  <img src="assets/images/logo.webp" width="400" alt="SIMD-F128 Logo">
  <br>
  <b>High-performance cross-platform 128-bit arithmetic for SIMD applications.</b>
</p>

# SIMD-F128

[![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![macOS](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![Windows](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![WASM](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![Mobile](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/Language-C11-00599C.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))

---

## Table of Contents

- [Introduction](#introduction)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [Quick Example](#quick-example)
- [Library Components](#library-components)
  - [simd_f128.h](#simd_f128h-core)
  - [simd_f128_consts.h](#simd_f128_constsh)
  - [simd_f128_io.h](#simd_f128_ioh)
- [API Reference](#api-reference)
- [Double-Double Arithmetic](#double-double-arithmetic)
- [Known Limitations](#known-limitations)
- [Examples](#examples)
- [Project Structure](#project-structure)
- [Build and Installation](#build-and-installation)
- [Contributing](#contributing)
- [License](#license)

---

## Introduction

**SIMD-F128** is a professional-grade, header-only C library for **128-bit (Double-Double)** floating-point arithmetic. It targets the precision gap between standard 64-bit IEEE 754 doubles and heavyweight arbitrary-precision libraries, providing approximately 31–32 decimal digits of accuracy with zero heap allocation overhead.

Designed for demanding workloads such as fractal rendering, physical simulations, and orbital mechanics — where double precision falls short but `libquadmath` or GMP would be excessive.

---

## Requirements

| Component | Requirement |
|---|---|
| C Standard | C11 or later (C99 compatible for scalar path) |
| Compiler | GCC 4.9+, Clang 3.5+, MSVC 2019+, Emscripten 3.0+ |
| Math library | `-lm` required on Linux/UNIX (for `fma()`) |

**Platform support:**

| Platform | Backend | Notes |
|---|---|---|
| Intel / AMD x86_64 (Haswell 2013+, Ryzen+) | AVX2 | Compile with `-mavx2 -mfma` or `-DSIMD_F128_AVX2=ON` |
| Apple Silicon (M1 / M2 / M3) | NEON | Auto-detected, no flag needed |
| ARM64 Linux (AWS Graviton, RPi 4/5 64-bit, Android NDK arm64) | NEON | Auto-detected, no flag needed |
| iOS (iPhone 5s+, all modern iPads) | NEON | Auto-detected, no flag needed |
| Windows on ARM (`_M_ARM64`) | NEON | Auto-detected, no flag needed |
| ARMv7 / AArch32 (RPi 2/3 32-bit, older Android, embedded) | Scalar | Add `-mfpu=neon-vfpv4 -mfloat-abi=hard` for hardware FMA |
| WebAssembly + SIMD128 (Chrome 91+, Firefox 89+, Safari 16.4+) | WASM-SIMD | Compile with `-msimd128` |
| WebAssembly Scalar (all browsers, Node.js, Deno) | Scalar | No flag needed, detected via `__EMSCRIPTEN__` |
| RISC-V (rv32/rv64, SiFive, VisionFive, SpacemiT) | Scalar | Auto-detected |
| PowerPC / POWER (ppc64le, AIX) | Scalar | Auto-detected |
| MIPS (32/64-bit) | Scalar | Auto-detected |
| All other targets (LoongArch, s390x, SPARC, etc.) | Scalar | Auto-detected |

The library auto-detects the backend at compile time via preprocessor macros. No manual configuration is needed; pass the appropriate flag and the correct path is selected automatically.

---

## Getting Started

### 1. Integration

Copy the desired `.h` files into your project. No pre-compilation or linking is required.

### 2. Implementation Guard

In **exactly one** source file, define the implementation macro before including the headers:

```c
#define SIMD_F128_IMPLEMENTATION
#include "simd_f128.h"
#include "simd_f128_io.h"
```

All other translation units include the headers without the macro.

---

## Quick Example

```c
#include <stdio.h>

#define SIMD_F128_IMPLEMENTATION
#include "simd_f128.h"
#include "simd_f128_consts.h"
#include "simd_f128_io.h"

int main() {
    simd_f128 r    = simd_f128_from_double(10.0);
    simd_f128 r2   = simd_f128_mul(r, r);
    simd_f128 area = simd_f128_mul(SIMD_F128_PI, r2);

    /* Output: 314.15926535897932384626433832795028 */
    printf("Circle Area: ");
    simd_f128_print(area);

    return 0;
}
```

---

## Library Components

### simd_f128.h (Core)

The central engine of the library. Implements the Double-Double representation and all fundamental arithmetic operations.

**Key properties:**

- **~106-bit mantissa** — roughly 31–32 decimal digits of precision.
- **Zero heap allocation** — all operations execute directly in CPU registers, suitable for tight inner loops.
- **Automatic SIMD dispatch** — selects AVX2 (`__m128d`) on Intel/AMD, NEON (`float64x2_t`) on ARM64/Apple Silicon, WASM-SIMD (`v128_t`) on the web, or falls back to optimized scalar C99.
- **Branchless implementation** — consistent execution time, no pipeline stalls.
- **Strict IEEE 754 foundation** — built on standard `double`, fully compatible with existing floating-point hardware.

```c
#define SIMD_F128_IMPLEMENTATION
#include "simd_f128.h"

int main() {
    simd_f128 a = simd_f128_from_double(1.234567890123456789);
    simd_f128 b = simd_f128_from_double(2.0);

    simd_f128 sum  = simd_f128_add(a, b);
    simd_f128 diff = simd_f128_sub(a, b);
    simd_f128 prod = simd_f128_mul(a, b);

    return 0;
}
```

---

### simd_f128_consts.h

Provides pre-computed, high-precision mathematical constants stored as Double-Double pairs. Each constant captures the full ~106-bit mantissa, avoiding the precision loss inherent in standard 64-bit initialisers.

```c
#include "simd_f128.h"
#include "simd_f128_consts.h"

int main() {
    simd_f128 pi     = SIMD_F128_PI;    /* 3.14159265358979323846... */
    simd_f128 e      = SIMD_F128_E;     /* 2.71828182845904523536... */
    simd_f128 sqrt_2 = SIMD_F128_SQRT2; /* 1.41421356237309504880... */

    return 0;
}
```

---

### simd_f128_io.h

Handles conversion between the internal Double-Double representation and human-readable decimal strings. Standard `printf` formatting cannot faithfully render 128-bit values; this header uses an iterative high-precision extraction algorithm to produce up to 32 correct decimal places.

```c
#define SIMD_F128_IMPLEMENTATION
#include "simd_f128.h"
#include "simd_f128_io.h"

int main() {
    simd_f128 val = simd_f128_from_double(3.141592653589793);

    /* Direct console output */
    simd_f128_print(val);

    /* String conversion for logging or UI */
    char buffer[128];
    simd_f128_to_string(buffer, sizeof(buffer), val);

    return 0;
}
```

---

## API Reference

### simd_f128.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_from_double` | `simd_f128 simd_f128_from_double(double d)` | Promote a `double` to 128-bit. `lo` is initialised to `0.0`. |
| `simd_f128_add` | `simd_f128 simd_f128_add(simd_f128 a, simd_f128 b)` | Double-Double addition via Knuth's TwoSum. |
| `simd_f128_sub` | `simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b)` | Double-Double subtraction (negates `b`, then adds). |
| `simd_f128_mul` | `simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b)` | Double-Double multiplication via Dekker's TwoProd + FMA. |

### simd_f128_consts.h

| Constant | Value (first 32 digits) |
|---|---|
| `SIMD_F128_PI` | 3.14159265358979323846264338327950... |
| `SIMD_F128_E` | 2.71828182845904523536028747135266... |
| `SIMD_F128_SQRT2` | 1.41421356237309504880168872420969... |
| `SIMD_F128_LN2` | 0.69314718055994530941723212145817... |

### simd_f128_io.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_print` | `void simd_f128_print(simd_f128 x)` | Print the value to `stdout` followed by a newline. |
| `simd_f128_to_string` | `void simd_f128_to_string(char* buf, size_t buf_size, simd_f128 x)` | Write up to 32 decimal digits into `buf`. `buf` must be at least 64 bytes. Handles `nan`, `inf`, and negative values. |

---

## Double-Double Arithmetic

SIMD-FP represents a value $x$ as the unevaluated sum of two IEEE 754 doubles:

$$x = x_{hi} + x_{lo}, \quad |x_{lo}| \leq \frac{1}{2} \, \text{ulp}(x_{hi})$$

This non-overlapping constraint provides ~106 bits of mantissa — approximately double the precision of a single `double`.

**Implementation basis:**

- **TwoSum (Knuth)** — error-free transformation for addition, capturing the exact rounding residual.
- **TwoProd (Dekker)** — error-free transformation for multiplication, exploiting FMA where available.
- **Normalisation** — each operation re-establishes the non-overlapping property before returning.

No memory allocation is required. The entire number lives in two registers.

---

## Known Limitations

**Operations available:** `add`, `sub`, `mul` only. Division, `sqrt`, and transcendental functions (`sin`, `cos`, `exp`, `log`) are not implemented. If these are required, they must be built on top of the provided primitives.

**Numerical range:** Identical to IEEE 754 `double` (~±1.8 × 10³⁰⁸). The library extends mantissa precision only; it does not extend the exponent range. Overflow and underflow occur at the same thresholds as standard `double`.

**Special values:** `NaN` and `Infinity` propagate through standard `double` rules. No extended multi-precision handling is applied — once `hi` becomes `NaN` or `Inf`, the result follows standard IEEE 754 behaviour.

**FMA dependency:** `simd_f128_mul` relies on a hardware-fused multiply-add instruction for correct error capture in `TwoProd`. On AArch64 and modern x86_64, FMA is always available in hardware. On ARMv7, FMA is available only with VFPv4 (Cortex-A7, A15, A17, A53 and later); compile with `-mfpu=neon-vfpv4` to ensure the compiler emits `VFMA` instead of a software fallback.

**Fixed precision:** The library is fixed at 128-bit (double-double). It is not a general-purpose arbitrary-precision library and cannot be extended to higher precision without redesign.

---

## Examples

Three runnable examples are provided under `examples/`. Each targets a specific aspect of the library.

**`basic_arithmetic.c`** — entry point for new users. Loads `SIMD_F128_PI` and `SIMD_F128_E` from `simd_f128_consts.h`, performs addition, subtraction, and multiplication, then prints all three results at full 32-digit precision via `simd_f128_io.h`.

**`precision_demo.c`** — demonstrates the core motivation for using this library. Adds `1e-17` to `1.0` using both a standard `double` and a `simd_f128`, then prints both results side by side. The `double` result silently loses the small value; the `simd_f128` result preserves it in the `lo` component. Works correctly on all three backends (AVX2, WASM, Scalar).

**`mandelbrot_core.c`** — a realistic use case. Runs the Mandelbrot iteration `z = z² + c` at a deep-zoom coordinate that exceeds 64-bit precision, using the correct escape condition (`|z|² > 4`). Reports whether the point escapes and prints the final `zx`/`zy` values at full precision.

---

## Project Structure

```text
.
├── assets/images/       # Logo and documentation media
├── examples/            # Runnable usage examples (see Examples above)
├── tests/               # Arithmetic unit tests (test_arithmetic.c)
├── .github/workflows/   # CI pipeline (test.yml)
├── simd_f128.h            # Core library — Double-Double arithmetic engine
├── simd_f128_consts.h     # High-precision mathematical constants
├── simd_f128_io.h         # String conversion and console output
├── CMakeLists.txt       # Cross-platform build configuration
└── LICENSE              # MIT License
```

---

## Build and Installation

SIMD-FP is header-only. The simplest integration is copying the `.h` files directly into your project.

### CMake

```bash
# Scalar backend (default — works everywhere)
cmake -S . -B build
cmake --build build

# AVX2 backend (Intel/AMD Haswell+)
cmake -S . -B build -DSIMD_F128_AVX2=ON
cmake --build build

# WebAssembly + SIMD128 (Chrome 91+, Firefox 89+, Safari 16.4+, Node.js 16+)
emcmake cmake -S . -B build -DSIMD_F128_WASM=ON
cmake --build build

# WebAssembly Scalar (maximum browser compatibility)
emcmake cmake -S . -B build
cmake --build build

# ARMv7 cross-compilation — optional flag for hardware FMA
cmake -S . -B build -DCMAKE_C_FLAGS="-mfpu=neon-vfpv4 -mfloat-abi=hard"
cmake --build build
```

AArch64 (Apple Silicon, Graviton, Android ARM64) requires no flags — NEON is auto-detected. Run tests after building:

```bash
ctest --test-dir build
```

---

## Contributing

I am still a learner in the field of numerical computing and low-level C programming. If you spot a precision bug, an incorrect algorithm, or an edge case I have missed — especially around FMA behaviour, normalisation stability, or platform-specific SIMD quirks — I would be genuinely grateful for the feedback. Every correction and suggestion is a lesson I would not have found on my own.

If you would like to help:

1. Open an **issue** to discuss bugs, inaccuracies, or potential improvements.
2. To contribute code, please **fork** the repository and open a **pull request** with a clear description of what was changed and why.
3. If you have expertise in Double-Double arithmetic or compiler-level float optimisation, architectural feedback is especially welcome.

Thank you for taking the time to read this far, and for helping make this project more correct.

---

## License

This project is licensed under the [MIT License](LICENSE) - see the [LICENSE](LICENSE) file for details
.