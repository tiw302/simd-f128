<p align="center">
  <img src="https://raw.githubusercontent.com/tiw302/simd-f128/master/assets/images/logo.webp" width="400" alt="simd-f128 Logo">
  <br>
  <b>High-performance, zero-allocation 128-bit floating-point arithmetic powered by hardware SIMD.</b>
</p>

# simd-f128

[![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![macOS](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![Windows](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![WASM](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![Mobile](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![PyPI](https://img.shields.io/pypi/v/simd-f128.svg)](https://pypi.org/project/simd-f128/)
[![NPM](https://img.shields.io/npm/v/@tiw302/simd-f128.svg)](https://www.npmjs.com/package/@tiw302/simd-f128)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/Language-C11-00599C.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Language](https://img.shields.io/badge/Language-C%2B%2B11-f34b7d.svg)](https://en.wikipedia.org/wiki/C%2B%2B11)
[![Header-Only](https://img.shields.io/badge/Library-Header--Only-brightgreen.svg)](#installation)
[![Zero-Allocation](https://img.shields.io/badge/Memory-Zero--Allocation-orange.svg)](#design-philosophy)
[![No-Dependencies](https://img.shields.io/badge/Dependencies-None-blueviolet.svg)](#introduction)
[![Last Commit](https://img.shields.io/github/last-commit/tiw302/simd-f128.svg)](https://github.com/tiw302/simd-f128/commits/master)

**[Read the Official Documentation: docs/index.md](https://tiw302.github.io/simd-f128/)**<br>
**[Try the Live WebAssembly Demo: https://tiw302.github.io/simd-f128/demo/](https://tiw302.github.io/simd-f128/demo/)**

> **Verified Compatibility — 11/11 Platforms Passing**

| Architecture | Platform | Verified Backend |
| :--- | :--- | :--- |
| **x86_64 (Modern)** | Linux / Windows | **AVX2** (Vectorized) |
| **x86_64 (Legacy)** | Linux / Windows | **SSE2** (Vectorized) |
| **ARM64 (Apple)** | macOS (M1/M2/M3) | **NEON** (Vectorized) |
| **ARM64 (Android)** | Mobile | **NEON** (Vectorized) |
| **ARMv7 (Android)** | Mobile | **Scalar** C11 |
| **WebAssembly** | Chrome / Node.js | **WASM-SIMD128** |
| **WebAssembly** | Universal Web | **WASM Scalar** |
| **RISC-V64** | Linux (QEMU) | **Scalar** C11 |
| **General Desktop** | Linux / Windows | **Scalar** C11 Fallback |

---

## Table of Contents

- [Introduction](#introduction)
  - [Why simd-f128?](#why-simd-f128)
  - [Design Philosophy](#design-philosophy)
- [Requirements & Toolchains](#requirements)
- [Build and Installation](#build-and-installation)
- [Library Components](#library-components)
  - [simd_f128.h (Core)](#simd_f128h-core)
  - [simd_f128_consts.h](#simd_f128_constsh)
  - [simd_f128_io.h](#simd_f128_ioh)
  - [simd_f128_math.h](#simd_f128_mathh)
  - [simd_f128_utils.h](#simd_f128_utilsh)
  - [simd_f128.hpp (C++)](#simd_f128hpp)
- [API Reference](#api-reference)
- [Performance & Benchmarks](#performance--benchmarks)
- [Double-Double Arithmetic](#double-double-arithmetic)
- [Examples](#examples)
- [Platform Support & CI Status](#platform-support--ci-status)
- [Language Bindings](#language-bindings)
- [Project Structure](#project-structure)

---

## Introduction

**simd-f128** is a professional-grade, header-only C library for **128-bit (Double-Double)** floating-point arithmetic, featuring automatic hardware SIMD acceleration (AVX2, NEON, WASM-SIMD). It explicitly targets the precision gap between standard 64-bit IEEE 754 doubles and heavyweight arbitrary-precision libraries like GMP.

By delivering **31-32 decimal digits of accuracy** with **zero heap allocation overhead**, `simd-f128` is purpose-built for demanding workloads—such as fractal rendering, physical simulations, and orbital mechanics. While the core engine is pure C11, it provides seamless native bindings for **C++, Python, WebAssembly, and Rust**, allowing developers across multiple ecosystems to easily overcome the limits of standard double precision.

---

## Why simd-f128?

Ever zoomed into a Mandelbrot set and watched the detail dissolve into grey mush? That's `double` precision dying — at zoom levels beyond ~10^-14, two distinct coordinates become the same value and the image collapses entirely. The same silent failure happens in long-running simulations, ill-conditioned linear algebra, and anywhere small errors compound over time.

The usual fixes each carry a significant cost:

| Option | Precision | Performance | Allocation | Portability |
|---|---|---|---|---|
| `double` | ~15 digits | Native Hardware | None | Universal |
| `long double` | 18-19 digits (x87) | Fast | None | Compiler-dependent |
| `__float128` (GCC) | ~33 digits | Emulated (Slow) | None | GCC/Clang only |
| GMP / MPFR | Arbitrary | Very Slow | **Heap** | Portable |
| **simd-f128** | **~31 digits** | **Hardware SIMD (Fast)**| **None** | **Universal** |

`__float128` gets close on precision but locks you into GCC/Clang and is noticeably slower due to software emulation. GMP/MPFR are powerful but heap-allocating inside a render loop is a non-starter.

simd-f128 occupies the exact gap: **it doubles usable precision with zero allocation, zero dependencies, and no compiler lock-in** — proven in practice by [mandelbrot-c](https://github.com/tiw302/mandelbrot-c), which achieves stable deep-zoom rendering at coordinates down to 10^-28, far beyond what standard `double` can represent.

---

## Design Philosophy

The library is built around three constraints that were never relaxed during development:

**Zero allocation.** Every operation executes entirely in CPU registers. There are no calls to `malloc`, no temporary buffers, and no GC pressure. This makes simd-f128 suitable for use inside tight render loops, interrupt handlers, and embedded firmware where heap allocation is prohibited.

**No configuration required.** The correct SIMD backend — AVX2, SSE2, NEON, WASM-SIMD, or scalar — is selected automatically at compile time based on the target architecture. If a specific hardware SIMD instruction set is not detected by the compiler, it seamlessly and safely falls back to a highly portable scalar implementation.

**Standard C foundation.** The library is built entirely on IEEE 754 `double` arithmetic and C11 standard library functions. It does not rely on compiler extensions, non-standard intrinsics outside of guarded `#ifdef` blocks, or platform-specific ABI assumptions. The scalar fallback compiles and produces correct results on any C99-compliant toolchain.

---

### Limitations & Technical Notes

**Double-Double vs IEEE 754 128-bit:**
Please note that `simd-f128` uses **Double-Double arithmetic** (an unevaluated sum of two standard 64-bit `double` values) to achieve approximately 31 decimal digits of precision. It is **not** a strictly compliant IEEE 754 `binary128` implementation.

While this approach offers massive performance benefits and is perfect for deeply zooming into fractals (like in [mandelbrot-c](https://github.com/tiw302/mandelbrot-c)), it is susceptible to **Catastrophic Cancellation** in specific scenarios (e.g., subtracting two nearly identical values). If you are building highly sensitive physics simulations or rigorous numerical analysis tools where IEEE 754 edge-case compliance is strictly required, a heavier library like GMP/MPFR or compiler-specific `__float128` may be more appropriate.

---

## Requirements

| Component | Requirement |
|---|---|
| C Standard | C11 or later (C99 compatible for scalar path) |
| C++ Standard | C++11 or later (for `simd_f128.hpp` only) |
| Compiler | GCC 4.9+, Clang 3.5+, MSVC 2019+, Emscripten 3.0+ |
| Math library | `-lm` required on Linux/UNIX (for `fma()`) |

---

## Verified Toolchains

The following toolchains are tested on every commit via CI. All others fall back to the scalar path and are expected to produce correct results.

| Toolchain | Version | Platform | Backend |
|---|---|---|---|
| GCC | 11+ | Linux x86_64 | Scalar, SSE2, AVX2 |
| GCC (aarch64-linux-gnu) | 11+ | Linux ARM64 (QEMU) | NEON |
| GCC (arm-linux-gnueabihf) | 11+ | Linux ARMv7 (QEMU) | Scalar + VFPv4 |
| GCC (riscv64-linux-gnu) | 11+ | Linux RISC-V64 (QEMU) | Scalar |
| Clang | 14+ | macOS Apple Silicon | NEON |
| Clang | 14+ | macOS Intel | Scalar, SSE2, AVX2 |
| MSVC | 2022 | Windows x64 | SSE2, AVX2 |
| Emscripten | 3.0+ | WASM (Node.js/Web) | WASM-SIMD, Scalar |

---

## Build and Installation

`simd-f128` can be integrated natively via C/C++ headers, Python, or JavaScript (WebAssembly).

### Python (PyPI)

```bash
pip install simd-f128
```

### JavaScript / Node.js (NPM)

```bash
npm install @tiw302/simd-f128
```

### C/C++ (Header Only)

simd-f128 is header-only. The simplest integration is copying the `include/` directory directly into your project, then defining the implementation macro in exactly one translation unit:

```c
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.h>
#include <simd_f128_io.h>   // optional
```

All other translation units include the headers without the macro.

For C++ projects, include the convenience wrapper instead:

```cpp
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.hpp>   // pulls in all headers automatically
```

### CMake

**System Install (Recommended)**
You can install the library system-wide to easily use `find_package` in other projects:

```bash
cmake -S . -B build
sudo cmake --install build
```

Then in your project's `CMakeLists.txt`:

```cmake
find_package(simd_fp REQUIRED)
target_link_libraries(my_app PRIVATE simd_fp::simd_fp)
```

**Local Build Options**

```bash
# Scalar backend (default - works everywhere)
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

# ARMv7 - optional flag for hardware FMA on VFPv4 cores
cmake -S . -B build -DCMAKE_C_FLAGS="-mfpu=neon-vfpv4 -mfloat-abi=hard"
cmake --build build
```

AArch64 (Apple Silicon, Graviton, Android ARM64) requires no flags - NEON is auto-detected. Run tests after building:

```bash
ctest --test-dir build
```

---

## Library Components

### simd_f128.h (Core)

The central engine of the library. Implements the Double-Double type and all fundamental arithmetic operations. All functions are `static inline` - no separate compilation unit is needed beyond the `SIMD_F128_IMPLEMENTATION` guard.

**Key properties:**

- **~106-bit mantissa** - roughly 31-32 decimal digits of precision.
- **Zero heap allocation** - all operations execute directly in CPU registers, suitable for tight inner loops.
- **Automatic SIMD dispatch** - selects AVX2/SSE2 (`__m128d`) on Intel/AMD, NEON (`float64x2_t`) on ARM64/Apple Silicon, WASM-SIMD (`v128_t`) on the web, or falls back to scalar C99.
- **Branch-free fast paths** - minimal branching (restricted to `Inf`/`NaN` guards) ensures consistent execution time and avoids pipeline stalls in the hot path.
- **Strict IEEE 754 foundation** - built on standard `double`, fully compatible with existing hardware.

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

### simd_f128_consts.h

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

### simd_f128_io.h

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

### simd_f128_math.h

Advanced mathematical functions built on top of the core Double-Double primitives. All functions are `static inline` and require no additional compilation unit.

**Algorithms used:**

- **`exp`** — range reduction to $N=16$ intervals followed by a high-degree Chebyshev minimax polynomial, then exact scaling via `ldexp` and a 16-entry lookup table. Handles overflow (`> 709.78`) and underflow explicitly.
- **`log`** — seeds from the standard `double` `log()`, then refines with 1 iteration of Halley's method, which is mathematically sufficient to recover all 31-32 digits due to cubic convergence.
- **`pow`** — computed as `exp(exp * log(base))`. Supports base-zero inputs and propagates `NaN` according to IEEE-754.
- **`sin`** — range-reduces to quadrant ($[-\pi/4, \pi/4]$) then evaluates a highly-tuned Chebyshev minimax polynomial.
- **`cos`** — range-reduces to quadrant ($[-\pi/4, \pi/4]$) then evaluates a highly-tuned Chebyshev minimax polynomial.
- **`sincos`** — computes both sine and cosine simultaneously, saving redundant Range Reduction and polynomial evaluation steps.

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

### simd_f128_utils.h

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

### simd_f128.hpp

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

---

## API Reference

### simd_f128.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_from_double` | `simd_f128 simd_f128_from_double(double d)` | Promote a `double` to 128-bit. `lo` is initialised to `0.0`. |
| `simd_f128_extract` | `void simd_f128_extract(simd_f128 x, double* hi, double* lo)` | Extract the `hi` and `lo` components into separate doubles. |
| `simd_f128_add` | `simd_f128 simd_f128_add(simd_f128 a, simd_f128 b)` | Double-Double addition via Knuth's TwoSum. |
| `simd_f128_sub` | `simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b)` | Double-Double subtraction (negates `b`, then adds). |
| `simd_f128_mul` | `simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b)` | Double-Double multiplication via Dekker's TwoProd + FMA. |
| `simd_f128_div` | `simd_f128 simd_f128_div(simd_f128 a, simd_f128 b)` | Double-Double division via Newton-Raphson reciprocal refinement. |
| `simd_f128_sqrt` | `simd_f128 simd_f128_sqrt(simd_f128 x)` | Square root via inverse-sqrt Newton-Raphson + residual correction. |

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

### simd_f128_math.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_exp` | `simd_f128 simd_f128_exp(simd_f128 x)` | `e^x`. Returns `+Inf` for `x > 709.78`, `0` for `x < -745`. |
| `simd_f128_log` | `simd_f128 simd_f128_log(simd_f128 x)` | Natural logarithm. Returns `NaN` for `x ≤ 0`. |
| `simd_f128_pow` | `simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp)` | `base^exp`. Correctly handles base zero, infinity, and NaN according to IEEE-754. |
| `simd_f128_sin` | `simd_f128 simd_f128_sin(simd_f128 x)` | Sine (radians). Best accuracy for moderate arguments. |
| `simd_f128_cos` | `simd_f128 simd_f128_cos(simd_f128 x)` | Cosine (radians). Best accuracy for moderate arguments. |
| `simd_f128_sincos` | `void simd_f128_sincos(simd_f128 x, simd_f128* s, simd_f128* c)` | Computes sine and cosine simultaneously. |

### simd_f128_utils.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_cmp` | `int simd_f128_cmp(simd_f128 a, simd_f128 b)` | Returns `-1` if `a < b`, `1` if `a > b`, `0` if equal. |
| `simd_f128_eq` | `int simd_f128_eq(simd_f128 a, simd_f128 b)` | `1` if `a == b`. |
| `simd_f128_gt` | `int simd_f128_gt(simd_f128 a, simd_f128 b)` | `1` if `a > b`. |
| `simd_f128_lt` | `int simd_f128_lt(simd_f128 a, simd_f128 b)` | `1` if `a < b`. |
| `simd_f128_ge` | `int simd_f128_ge(simd_f128 a, simd_f128 b)` | `1` if `a >= b`. |
| `simd_f128_le` | `int simd_f128_le(simd_f128 a, simd_f128 b)` | `1` if `a <= b`. |
| `simd_f128_abs` | `simd_f128 simd_f128_abs(simd_f128 x)` | Absolute value. Correctly handles `-0.0` in the `lo` component. |
| `simd_f128_min` | `simd_f128 simd_f128_min(simd_f128 a, simd_f128 b)` | Returns the lesser of `a` and `b`. |
| `simd_f128_max` | `simd_f128 simd_f128_max(simd_f128 a, simd_f128 b)` | Returns the greater of `a` and `b`. |

### simd_f128.hpp (C++ only)

| Symbol | Kind | Description |
|---|---|---|
| `f128::float128` | Class | C++ wrapper around `simd_f128`. |
| `f128::float128(double)` | Constructor | Construct from a `double`. |
| `f128::float128(simd_f128)` | Constructor | Construct from a raw `simd_f128`. |
| `float128::extract(hi, lo)` | Method | Extract `hi` and `lo` components. |
| `+`, `-`, `*`, `/` | Operators | Arithmetic operators. |
| `+=`, `-=`, `*=`, `/=` | Operators | Compound assignment operators. |
| `==`, `!=`, `<`, `>`, `<=`, `>=` | Operators | Comparison operators. |
| `operator-()` | Unary | Negation. |
| `float128::to_string()` | Method | Returns `std::string` with 32-digit representation. |
| `operator<<` | Stream | `std::ostream` integration. |
| `f128::exp`, `f128::log`, `f128::pow` | Free functions | Transcendental math. |
| `f128::sin`, `f128::cos`, `f128::sqrt`, `f128::abs` | Free functions | Trigonometric and utility math. |
| `f128::pi`, `f128::e`, `f128::sqrt2`, `f128::ln2` | Constants | High-precision constants as `float128`. |

---

### Precision Demonstration & Test Results

The core advantage of `simd-f128` is preserving small values that standard 64-bit doubles silently discard. All operations execute strictly within SIMD registers without heap allocation.

Here is an actual test run and precision comparison from the `Extreme Performance` build:

```console
~/Public/simd-f128 master* ⇡
❯ ctest --test-dir build -C Release
Test project /simd-f128/build
    Start 1: arithmetic_test
1/2 Test #1: arithmetic_test ..................   Passed    0.00 sec
    Start 2: arithmetic_test_cpp
2/2 Test #2: arithmetic_test_cpp ..............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 2

~/Public/simd-f128 master* ⇡
❯ ./build/example_precision
--- precision comparison: double vs simd-fp ---

[double]  1.0 + 1e-17 = 1.00000000000000000000
          precision lost: yes

[simd-fp] 1.0 + 1e-17 = 1.00000000000000001000000000000000
          precision lost: no


~/Public/simd-f128 master* ⇡
❯ ./build/example_mandelbrot
--- mandelbrot core loop (128-bit precision) ---

did not escape after 500 iterations (point is inside the Mandelbrot set)

final |z| components:
  zx = -0.78124578860038387003505655582563
  zy = 0.35443468442007221298624089031401
```

## Performance & Benchmarks

Because `simd-f128` operations are purely CPU-register bound, they are extremely fast. 

### 1. Comparative Speed vs `__float128`

While raw nanoseconds are interesting, a direct comparison against `__float128` demonstrates the massive advantage of hardware SIMD over software emulation. The test simulates loop-carried dependency latency (e.g., `a = a + b`) simulating tight inner-loops in numerical algorithms. Tests run for 10,000,000 operations.

| Data Type | Add (ms) | Mul (ms) | Div (ms) |
|---|---|---|---|
| `double` (64-bit) | 9.24 | 9.23 | 41.83 |
| `long double` (x87) | 20.70 | 20.66 | 48.49 |
| `__float128` (GCC) | 153.37 | 193.23 | 325.37 |
| **`simd-f128` (AVX2)** | **99.44** | **74.46** | **207.98** |

<details>
<summary><b>View raw console output from bench_compare</b></summary>

```console
$ ./build/benchmarks/bench_compare

simd-f128 Manual Benchmark Comparison
Iterations: 10000000 operations per test (latency mode)

| Data Type          | Add (ms) | Mul (ms) | Div (ms) |
|--------------------|----------|----------|----------|
|--------------------|----------|----------|----------|
| double (64-bit)    |     9.24 |     9.23 |    41.83 |
| long double (x87)  |    20.70 |    20.66 |    48.49 |
| __float128 (GCC)   |   153.37 |   193.23 |   325.37 |
| simd-f128 (SIMD)   |    99.44 |    74.46 |   207.98 |
```

</details>

**Analysis:**
`simd-f128` on AVX2 decisively outperforms GCC's software-emulated `__float128`. Specifically, **multiplication is 2.59x faster**, addition is 1.54x faster, and division is 1.56x faster. This is achieved through the aggressive use of Hardware FMA (Fused Multiply-Add), which rapidly resolves Dekker's split algorithms natively in silicon without relying on slower branching software emulation.

### 2. WebAssembly (In-Browser) Benchmarks

The library ships with dual WebAssembly modules to maximise both performance and compatibility. The benchmarks below reflect 1,000,000 continuous `simd_f128_mul` operations running entirely inside the V8 JavaScript engine (Chrome).

| Module Type | Time (ms) | Notes |
|---|---|---|
| **WASM-SIMD128** | ~295 ms | Native 128-bit SIMD processing inside the browser. |
| **WASM-Scalar** | ~481 ms | Fallback for older browsers without SIMD support. |
| Native JS `Number` | ~1.5 ms | Native 64-bit precision (loss of 15 digits of precision). |

**Takeaway:** `WASM-SIMD128` achieves a **~1.6x speedup** over scalar WASM inside the browser. While native JS `Number` is incredibly fast due to JIT compilation of single hardware instructions, it completely fails to preserve precision past 15 digits. `simd-f128` enables software running in the browser to maintain 32-digit precision with highly acceptable latency for real-time visualization and mathematical processing.

### 3. Raw Speed (Google Benchmark)

A single `simd_f128_mul` completes in ~10 nanoseconds, and advanced math functions run in the ~170-490ns range.

```console
Run on (12 X 3266.69 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 512 KiB (x6)
  L3 Unified 16384 KiB (x1)
-----------------------------------------------------------
Benchmark                 Time             CPU   Iterations
-----------------------------------------------------------
BM_SimdF128_Add        11.7 ns         11.7 ns     60057911
BM_SimdF128_Mul        10.1 ns         10.1 ns     69579904
BM_SimdF128_Div        2.87 ns         2.86 ns    244206304
BM_SimdF128_Sqrt       6.05 ns         6.04 ns    115940003
BM_SimdF128_Exp         192 ns          192 ns      3646032
BM_SimdF128_Log         240 ns          240 ns      2920704
BM_SimdF128_Sin         192 ns          192 ns      3645663
BM_SimdF128_Cos         200 ns          199 ns      3510110
BM_SimdF128_Atan        402 ns          401 ns      1743733
BM_SimdF128_Pow         492 ns          491 ns      1426559
```

---

## Double-Double Arithmetic

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

---

## Examples

Three runnable examples are provided under `examples/`.

**`basic_arithmetic.c`** — entry point for new users. Loads `SIMD_F128_PI` and `SIMD_F128_E` from `simd_f128_consts.h`, performs addition, subtraction, and multiplication, then prints all three results at full 32-digit precision.

**`precision_demo.c`** — demonstrates the core motivation for using this library. Adds `1e-17` to `1.0` using both a standard `double` and a `simd_f128`, then prints both results side by side. The `double` result silently loses the small value; the `simd_f128` result preserves it in the `lo` component.

**`mandelbrot_core.c`** — a realistic use case. Runs the Mandelbrot iteration `z = z^2 + c` at a deep-zoom coordinate that exceeds 64-bit precision, with the correct escape condition (`|z|^2 > 4`). Reports whether the point escapes and prints the final `zx`/`zy` values at full precision.

Quick example — circle area at 32-digit precision:

```c
#include <stdio.h>

#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.h>
#include <simd_f128_consts.h>
#include <simd_f128_io.h>

int main() {
    simd_f128 r    = simd_f128_from_double(10.0);
    simd_f128 r2   = simd_f128_mul(r, r);
    simd_f128 area = simd_f128_mul(SIMD_F128_PI, r2);

    // output: 314.15926535897932384626433832795028
    printf("Circle Area: ");
    simd_f128_print(area);

    return 0;
}
```

Same example using the C++ wrapper:

```cpp
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.hpp>
#include <iostream>

int main() {
    f128::float128 r(10.0);
    f128::float128 area = f128::pi * r * r;

    // output: 314.15926535897932384626433832795028
    std::cout << "Circle Area: " << area << "\n";

    return 0;
}
```

---

## Platform Support & CI Status

Every commit is tested across all backends via GitHub Actions. The table below maps each workflow to the platforms and backends it covers.

| Workflow | Platform | Backend | Runner |
|---|---|---|---|
| [![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml) | Linux x86_64 | Scalar, AVX2 | `ubuntu-latest` |
| [![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml) | Linux ARM64, ARMv7, RISC-V64 | NEON, Scalar | `ubuntu-latest` + QEMU |
| [![macOS](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml) | Apple Silicon (M1/M2/M3) | NEON | `macos-latest` |
| [![macOS](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml) | macOS Intel | Scalar, AVX2 | `macos-13` |
| [![Windows](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml) | Windows x64 (MSVC) | Scalar | `windows-latest` |
| [![WASM](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml) | WebAssembly (Node.js) | WASM-SIMD, Scalar | `ubuntu-latest` + Emscripten |
| [![Mobile](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml) | Android ARM64, Android ARMv7 | NEON, Scalar | `ubuntu-latest` + QEMU |

---

## Language Bindings

`simd-f128` is designed to provide 128-bit precision not just to C/C++, but to higher-level ecosystems.

### Python
Using `pybind11`, the library is exposed as a native CPython extension, bringing 31-digit precision directly into Python scripts.
```python
import simd_f128 as f128

a = f128.from_string("3.14159265358979323846")
b = f128.from_double(2.0)
print((a * b).to_string())
```

### JavaScript / WebAssembly
Compiled via Emscripten, the JS bindings automatically select between `WASM-SIMD128` and `WASM-Scalar` depending on the user's browser support, providing 31-digit precision directly in the browser or Node.js.

### Rust
A fully memory-safe Rust wrapper (via `cc` and `bindgen`), exposing the C functions safely through idiomatic Rust structs and operator overloads.

---

## Project Structure

```text
.
├── assets/images/        # Logo and documentation media
├── benchmarks/           # Performance benchmarks (Google Benchmark & Native)
├── examples/             # Runnable usage examples
│   ├── basic_arithmetic.c
│   ├── precision_demo.c
│   └── mandelbrot_core.c
├── tests/                # Arithmetic unit tests (C and C++)
├── .github/workflows/    # CI pipelines (linux, macos, windows, wasm, mobile)
├── include/              # Core library and headers
│   ├── simd_f128.h           # Double-Double arithmetic engine
│   ├── simd_f128_consts.h    # High-precision mathematical constants
│   ├── simd_f128_io.h        # String conversion and console output
│   ├── simd_f128_math.h      # Advanced mathematical functions (exp, log, sin, cos, pow)
│   ├── simd_f128_utils.h     # Comparison and utility functions (cmp, abs, min, max)
│   └── simd_f128.hpp         # Modern C++ wrapper with operator overloading
├── js/                   # JavaScript bindings and WebAssembly module
├── python/               # Python bindings (pybind11)
├── rust/                 # Rust bindings (FFI via cc)
├── CMakeLists.txt        # Cross-platform build configuration
└── LICENSE               # MIT License
```

---

## Used By

| Project | Description |
|---|---|
| [mandelbrot-c](https://github.com/tiw302/mandelbrot-c) | Deep-zoom Mandelbrot renderer in C, using simd-f128 for 128-bit precision coordinates |



## Development Methodology & AI Assistance

Building a memory-safe, mathematically robust SIMD library requires managing incredibly complex edge cases—from vectorized bit-manipulation to IEEE 754 catastrophic cancellation bounds.

To achieve this level of stability and performance, this project was architected and rigorously verified in collaboration with **Advanced Agentic AI**. AI was specifically utilized to:

- Stress-test the Double-Double arithmetic engine against extreme floating-point edge cases (subnormals, infinities, NaN propagation).
- Assist in planning the memory layout and cross-platform SIMD abstraction (AVX2, NEON, WASM).
- Automate the generation of robust cross-platform CI/CD pipelines (Linux, macOS, Windows, Mobile, WebAssembly).

However, **human agency remains at the core of this project**. Every single line of code generated or suggested was manually inspected, audited, and strictly verified. The core architecture, mathematical algorithms, and memory design were meticulously human-planned. This hybrid approach—combining human architectural vision with AI-driven debugging and verification—allowed us to push the boundaries of performance and reliability in a modern C library without compromising accuracy or code ownership.

---

## Author's Note

I'm just a kid building projects as a hobby. Thank you for showing interest in my little library! It really means a lot to me. :)

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

This project is licensed under the [MIT License](LICENSE) - see the [LICENSE](LICENSE) file for details.
