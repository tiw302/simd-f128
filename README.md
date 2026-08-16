<div align="center">

<img src="https://raw.githubusercontent.com/tiw302/simd-f128/master/assets/images/logo.png" width="600" alt="simd-f128 Logo">

**High-performance, zero-allocation 128-bit floating-point arithmetic powered by hardware SIMD.**

[![Linux](https://img.shields.io/github/actions/workflow/status/tiw302/simd-f128/linux.yml?label=Linux%20Build&logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml) [![macOS](https://img.shields.io/github/actions/workflow/status/tiw302/simd-f128/macos.yml?label=macOS%20Build&logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml) [![Windows](https://img.shields.io/github/actions/workflow/status/tiw302/simd-f128/windows.yml?label=Windows%20Build&logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml) [![Rust](https://img.shields.io/github/actions/workflow/status/tiw302/simd-f128/rust.yml?label=Rust%20Bindings&logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/actions/workflows/rust.yml) [![WASM](https://img.shields.io/github/actions/workflow/status/tiw302/simd-f128/wasm.yml?label=WebAssembly%20Build&logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml) [![Mobile](https://img.shields.io/github/actions/workflow/status/tiw302/simd-f128/mobile.yml?label=Mobile%20Build&logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml)

[![PyPI](https://img.shields.io/pypi/v/simd-f128.svg?logo=pypi&logoColor=white)](https://pypi.org/project/simd-f128/) [![NPM](https://img.shields.io/npm/v/@tiw302/simd-f128.svg?logo=npm&logoColor=white)](https://www.npmjs.com/package/@tiw302/simd-f128) [![Crates.io](https://img.shields.io/crates/v/simd-f128.svg?logo=rust&logoColor=white)](https://crates.io/crates/simd-f128)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?logo=opensourceinitiative&logoColor=white)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/Language-C11-00599C.svg?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Language](https://img.shields.io/badge/Language-C%2B%2B11-f34b7d.svg?logo=cplusplus&logoColor=white)](https://en.wikipedia.org/wiki/C%2B%2B11)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows%20%7C%20Web%20%7C%20Mobile-lightgrey.svg?logo=linux&logoColor=white)](#verified-compatibility--1717-ci-jobs-passing)
[![SIMD](https://img.shields.io/badge/SIMD-AVX2%20%7C%20SSE2%20%7C%20NEON%20%7C%20WASM--SIMD-FF6B35.svg?logo=intel&logoColor=white)](#verified-compatibility--1717-ci-jobs-passing)
![GitHub repo size](https://img.shields.io/github/repo-size/tiw302/simd-f128?logo=github&logoColor=white)
[![Last Commit](https://img.shields.io/github/last-commit/tiw302/simd-f128.svg?logo=github&logoColor=white)](https://github.com/tiw302/simd-f128/commits/master)

</div>

# simd-f128

**[Read the Official Documentation: docs/index.md](https://tiw302.github.io/simd-f128/)**<br>
**[Try the Live WebAssembly Demo: https://tiw302.github.io/simd-f128/demo/](https://tiw302.github.io/simd-f128/demo/)**

> **Verified Compatibility — 17/17 CI Jobs Passing**

| Architecture | Platform | CI Workflow | Backend |
| :--- | :--- | :--- | :--- |
| **x86_64** | Linux | `linux.yml` | **Scalar** |
| **x86_64** | Linux | `linux.yml` | **SSE2** (Vectorized) |
| **x86_64** | Linux | `linux.yml` | **AVX2** (Vectorized) |
| **x86_64** | Windows (MSVC) | `windows.yml` | **Scalar** |
| **x86_64** | Windows (MSVC) | `windows.yml` | **AVX2** (Vectorized) |
| **ARM64 (Apple)** | macOS (M-series) | `macos.yml` | **NEON** (Vectorized) |
| **ARM64** | Linux/Android (QEMU) | `mobile.yml` | **NEON** (Vectorized) |
| **ARMv7** | Linux/Android (QEMU) | `mobile.yml` | **Scalar** + VFPv4 |
| **RISC-V64** | Linux (QEMU) | `linux.yml` | **Scalar** C11 |
| **WebAssembly** | Node.js | `wasm.yml` | **WASM-SIMD128** |
| **WebAssembly** | Node.js | `wasm.yml` | **WASM Scalar** |
| **Python bindings** | Linux | `linux.yml` | Python 3.10 extension |
| **Rust bindings** | Linux | `rust.yml` | FFI via `cc` |
| **Rust bindings** | Windows | `rust.yml` | FFI via `cc` |
| **Rust bindings** | macOS | `rust.yml` | FFI via `cc` |

---

## Table of Contents

- [simd-f128](#simd-f128)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Who is this for?](#who-is-this-for)
  - [Why simd-f128?](#why-simd-f128)
    - [Performance Benchmarks](#performance-benchmarks)
      - [Advanced Math Functions (Nanosecond Latency)](#advanced-math-functions-nanosecond-latency)
  - [Design Philosophy](#design-philosophy)
    - [Limitations \& Technical Notes](#limitations--technical-notes)
  - [Requirements](#requirements)
  - [Verified Toolchains](#verified-toolchains)
  - [Build and Installation](#build-and-installation)
    - [Language Bindings](#language-bindings)
    - [C/C++ (Header Only)](#cc-header-only)
    - [CMake](#cmake)
  - [Library Components](#library-components)
  - [Documentation](#documentation)
    - [simd\_f128.h](#simd_f128h)
    - [simd\_f128\_consts.h](#simd_f128_constsh)
    - [simd\_f128\_io.h](#simd_f128_ioh)
    - [simd\_f128\_math.h](#simd_f128_mathh)
    - [simd\_f128\_utils.h](#simd_f128_utilsh)
    - [simd\_f128.hpp (C++ only)](#simd_f128hpp-c-only)
    - [Precision Demonstration \& Test Results](#precision-demonstration--test-results)
  - [Performance \& Benchmarks](#performance--benchmarks)
    - [1. Comparative Speed vs `__float128`](#1-comparative-speed-vs-__float128)
    - [2. WebAssembly (In-Browser) Benchmarks](#2-webassembly-in-browser-benchmarks)
    - [3. Raw Speed (Google Benchmark)](#3-raw-speed-google-benchmark)
  - [Double-Double Arithmetic](#double-double-arithmetic)
  - [Examples](#examples)
    - [C (`examples/c/`)](#c-examplesc)
    - [C++ (`examples/cpp/`)](#c-examplescpp)
    - [Python (`examples/python/`)](#python-examplespython)
    - [JavaScript (`examples/js/`)](#javascript-examplesjs)
    - [Rust (`examples/rust/`)](#rust-examplesrust)
  - [Platform Support \& CI Status](#platform-support--ci-status)
  - [Language Bindings](#language-bindings-1)
    - [Python](#python)
    - [JavaScript / WebAssembly](#javascript--webassembly)
    - [Rust](#rust)
  - [Project Structure](#project-structure)
  - [Used By](#used-by)
  - [Author's Note](#authors-note)
  - [Contributing](#contributing)
  - [License](#license)

---

## Introduction

**simd-f128** is a professional-grade, header-only C library for **128-bit (Double-Double)** floating-point arithmetic, featuring automatic hardware SIMD acceleration (AVX2, NEON, WASM-SIMD). It explicitly targets the precision gap between standard 64-bit IEEE 754 doubles and heavyweight arbitrary-precision libraries like GMP.

By delivering **31-32 decimal digits of accuracy** with **zero heap allocation overhead**, `simd-f128` is purpose-built for demanding workloads—such as fractal rendering, physical simulations, and orbital mechanics. While the core engine is pure C11, it provides seamless native bindings for **C++, Python, WebAssembly, and Rust**, allowing developers across multiple ecosystems to easily overcome the limits of standard double precision.

---

## Who is this for?

**simd-f128 is a good fit if you are:**

- a **graphics / demo-scene developer** pushing Mandelbrot or Julia-set renders past the 64-bit precision wall (~10⁻¹⁴ zoom) and need stable coordinates without GMP overhead
- a **numerical / scientific computing developer** whose simulations accumulate floating-point error over long time steps — orbital mechanics, n-body, RK4 integrators — and need more mantissa without rewriting in Python/Julia
- a **game / engine developer** doing high-precision physics or world-space transforms where `double` cancellation becomes visible at large coordinates
- a **web developer or data scientist** who needs more than 15 significant digits in the browser or in a Python script, and wants a compiled WASM/extension rather than a pure-JS BigDecimal library
- a **C/C++ library author** who needs a drop-in 128-bit scalar type that is header-only, zero-allocation, and works on every CI target without compiler flags

**simd-f128 is probably not what you need if:**

- you need arbitrary precision (thousands of digits) → use GMP/MPFR
- you need strict IEEE 754 `binary128` compliance for standards-conformant output → use GCC `__float128` or a software quad library
- you are subtracting two nearly-equal values in a numerically sensitive inner loop without knowing about catastrophic cancellation → compensated summation (Kahan) or a different algorithm is the real fix

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

### Performance Benchmarks

Below is a benchmark comparison of basic arithmetic operations running on **10,000,000 iterations** (latency mode):

| Data Type | Add (ms) | Mul (ms) | Div (ms) | Relative Multiplication Speed |
|---|---|---|---|---|
| `double` (64-bit) | 8.80 | 8.07 | 33.06 | 1.00x (Baseline) |
| `long double` (x87) | 16.66 | 17.28 | 38.58 | 0.47x |
| `__float128` (GCC) | 152.31 | 189.81 | 276.00 | 0.04x |
| **simd-f128 (SIMD)** | **73.60** | **56.15** | **159.70** | **0.14x (3.38x faster than GCC)** |

![simd-f128 Benchmark Comparison](https://raw.githubusercontent.com/tiw302/simd-f128/master/assets/benchmarks/benchmark.png)

As shown, `simd-f128` is **1.7x to 3.3x faster** than GCC's software-emulated `__float128`, making it the highest-performance choice for 128-bit precision.

#### Advanced Math Functions (Nanosecond Latency)

![simd-f128 Math Functions Latency](https://raw.githubusercontent.com/tiw302/simd-f128/master/assets/benchmarks/math_latency.png)

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

| Toolchain | Version | Platform | Backend |
|---|---|---|---|
| GCC | 11+ | Linux x86_64 | Scalar, SSE2, AVX2 |
| GCC (aarch64-linux-gnu) | 11+ | Linux ARM64 (QEMU) | NEON |
| GCC (arm-linux-gnueabihf) | 11+ | Linux ARMv7 (QEMU) | Scalar + VFPv4 |
| GCC (riscv64-linux-gnu) | 11+ | Linux RISC-V64 (QEMU) | Scalar |
| Clang | 14+ | macOS Apple Silicon | NEON |
| MSVC | 2022 | Windows x64 | Scalar, AVX2 |
| Emscripten | 3.0+ | WASM (Node.js/Web) | WASM-SIMD, Scalar |

---

## Build and Installation

`simd-f128` can be integrated natively via C/C++ headers, Python, JavaScript (WebAssembly), and Rust.

### Language Bindings

Native bindings are available via standard package managers:

| Language | Package Manager | Install Command |
|---|---|---|
| **Python** | [PyPI](https://pypi.org/project/simd-f128/) | `pip install simd-f128` |
| **Node.js** | [npm](https://www.npmjs.com/package/@tiw302/simd-f128) | `npm install @tiw302/simd-f128` |
| **Rust** | [Crates.io](https://crates.io/crates/simd-f128) | `cargo add simd-f128` |

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
find_package(simd_f128 REQUIRED)
target_link_libraries(my_app PRIVATE simd_f128::simd_f128)
```

**FetchContent (Alternative)**

If you prefer not to install the library system-wide, you can pull it directly into your build:

```cmake
include(FetchContent)

FetchContent_Declare(
    simd_f128
    GIT_REPOSITORY https://github.com/tiw302/simd-f128.git
    GIT_TAG        v1.5.4
)
FetchContent_MakeAvailable(simd_f128)

target_link_libraries(my_app PRIVATE simd_f128::simd_f128)
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

All headers are `static inline` / header-only. A quick summary of what each header provides:

| Header | Purpose |
|---|---|
| `simd_f128.h` | Core type and arithmetic (add, sub, mul, div, sqrt) |
| `simd_f128_consts.h` | Pre-computed constants (π, e, √2, ln2 at full 106-bit precision) |
| `simd_f128_io.h` | String parsing and `printf`-style output at 32-digit precision |
| `simd_f128_math.h` | Transcendental and trig functions (exp, log, sin, cos, atan, sinh, floor, …) |
| `simd_f128_utils.h` | Comparison operators (lt, eq, ge, …) and abs/min/max |
| `simd_f128_matrix.h` | mat2/mat3/mat4 and vec2/vec3/vec4 at 128-bit precision |
| `simd_f128_random.h` | xoshiro256** PRNG producing uniform `simd_f128` in `[0, 1)` |
| `simd_f128_vector.h` | 4-lane vectorized ops (`simd_f128x4`) using AVX2 |
| `simd_f128.hpp` | C++ wrapper with operator overloading and `std::ostream` integration |
| `simd_f128_complex.hpp` | `std::complex<f128::float128>` interoperability |
| `simd_f128_eigen.hpp` | Eigen `NumTraits` so `float128` works in `Eigen::Matrix` |

**→ Full per-header documentation with code examples: [docs/components.md](docs/components.md)**

## Documentation

- **[Components Guide](docs/components.md)**: Detailed overview of all modules and their dependencies.
- **[API Reference](docs/api_reference.md)**: Full function-level reference with signatures and descriptions.
- **[Math Theory & Architecture](docs/math_theory.md)**: In-depth explanation of Double-Double arithmetic and SIMD optimizations.

Quick lookup — core functions:

| Function | Description |
|---|---|
| `simd_f128_from_double(d)` | Promote `double` to 128-bit. |
| `simd_f128_add(a, b)` | Double-Double addition (TwoSum). |
| `simd_f128_sub(a, b)` | Subtraction. |
| `simd_f128_mul(a, b)` | Multiplication (TwoProd + FMA). |
| `simd_f128_div(a, b)` | Division (Newton-Raphson). |
| `simd_f128_sqrt(x)` | Square root (Newton-Raphson + residual). |
| `simd_f128_exp(x)` | `e^x`. |
| `simd_f128_log(x)` | Natural log. |
| `simd_f128_sin(x)` / `simd_f128_cos(x)` | Sine / cosine (radians). |
| `simd_f128_print(x)` | Print to stdout at 32 digits. |
| `simd_f128_to_string(buf, n, x)` | Write to string buffer. |

---

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
| `simd_f128_log` | `simd_f128 simd_f128_log(simd_f128 x)` | Natural log. Returns `NaN` for `x ≤ 0`. |
| `simd_f128_log2` | `simd_f128 simd_f128_log2(simd_f128 x)` | Base-2 log. |
| `simd_f128_log10` | `simd_f128 simd_f128_log10(simd_f128 x)` | Base-10 log. |
| `simd_f128_pow` | `simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp)` | `base^exp`. Handles base zero, infinity, and NaN per IEEE-754. |
| `simd_f128_cbrt` | `simd_f128 simd_f128_cbrt(simd_f128 x)` | Cube root. |
| `simd_f128_sin` | `simd_f128 simd_f128_sin(simd_f128 x)` | Sine (radians). |
| `simd_f128_cos` | `simd_f128 simd_f128_cos(simd_f128 x)` | Cosine (radians). |
| `simd_f128_tan` | `simd_f128 simd_f128_tan(simd_f128 x)` | Tangent (radians). |
| `simd_f128_sincos` | `void simd_f128_sincos(simd_f128 x, simd_f128* s, simd_f128* c)` | Computes sine and cosine in a single pass. |
| `simd_f128_atan` | `simd_f128 simd_f128_atan(simd_f128 x)` | Arctangent. |
| `simd_f128_atan2` | `simd_f128 simd_f128_atan2(simd_f128 y, simd_f128 x)` | `atan(y/x)` with quadrant correction. |
| `simd_f128_asin` | `simd_f128 simd_f128_asin(simd_f128 x)` | Arcsine. Domain: `[-1, 1]`. |
| `simd_f128_acos` | `simd_f128 simd_f128_acos(simd_f128 x)` | Arccosine. Domain: `[-1, 1]`. |
| `simd_f128_sinh` | `simd_f128 simd_f128_sinh(simd_f128 x)` | Hyperbolic sine. |
| `simd_f128_cosh` | `simd_f128 simd_f128_cosh(simd_f128 x)` | Hyperbolic cosine. |
| `simd_f128_tanh` | `simd_f128 simd_f128_tanh(simd_f128 x)` | Hyperbolic tangent. |
| `simd_f128_floor` | `simd_f128 simd_f128_floor(simd_f128 x)` | Floor. |
| `simd_f128_ceil` | `simd_f128 simd_f128_ceil(simd_f128 x)` | Ceiling. |
| `simd_f128_trunc` | `simd_f128 simd_f128_trunc(simd_f128 x)` | Truncate toward zero. |
| `simd_f128_round` | `simd_f128 simd_f128_round(simd_f128 x)` | Round half-away from zero. |
| `simd_f128_fmod` | `simd_f128 simd_f128_fmod(simd_f128 a, simd_f128 b)` | Floating-point remainder. |

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
--- precision comparison: double vs simd-f128 ---

[double]  1.0 + 1e-17 = 1.00000000000000000000
          precision lost: yes

[simd-f128] 1.0 + 1e-17 = 1.00000000000000001000000000000000
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

simd-f128 represents a value as the unevaluated sum of two IEEE 754 doubles — `hi + lo` where `|lo| ≤ ½ ulp(hi)`. This non-overlapping constraint gives ~106 bits of mantissa (~31-32 decimal digits).

Core algorithms: **TwoSum** (Knuth) for addition, **TwoProd** (Dekker) + FMA for multiplication, **Newton-Raphson** for division and sqrt.

**→ Full theory and known limitations: [docs/math_theory.md](docs/math_theory.md)**

---

## Examples

All examples are under `examples/` and build via `./build.sh examples` or `cmake -DSIMD_F128_BUILD_EXAMPLES=ON`.

### C (`examples/c/`)

**`basic_arithmetic.c`** — starting point. Loads `SIMD_F128_PI` and `SIMD_F128_E`, does add/sub/mul, prints at full 32-digit precision.

**`precision_demo.c`** — side-by-side comparison of `double` vs `simd_f128` on `1.0 + 1e-17`. The double silently drops the small value; simd_f128 keeps it in the `lo` component.

**`mandelbrot_core.c`** — runs `z = z² + c` at a deep-zoom coordinate past the 64-bit precision boundary. Checks the escape condition `|z|² > 4` and prints the final `zx`/`zy` at full precision.

**`matrix_transform.c`** — 4×4 transform matrix ops using `simd_f128_matrix.h`. Builds a rotation/scale matrix and multiplies it against a vec4, showing the precision advantage on accumulated transform errors.

**`chaotic_pendulum.c`** — simulates a double pendulum with RK4 integration. A good stress-test for accumulated floating-point error — two runs with slightly different initial conditions diverge, demonstrating why 128-bit precision matters for long-horizon simulations.

**`crypto_large_integer.c`** — big-integer-style modular arithmetic using double-double pairs as a precision substrate, relevant to cryptography primitives that need more than 53 bits of mantissa.

### C++ (`examples/cpp/`)

**`cpp_operator_overload.cpp`** — demo of `f128::float128` from `simd_f128.hpp`. Natural arithmetic (`+`, `*`, `/`), comparison operators, `std::ostream` output, and free math functions (`f128::sin`, `f128::exp`, etc.).

### Python (`examples/python/`)

**`python_vanishing_gradient.py`** — simulates the vanishing gradient problem in a deep neural network using 128-bit precision. Compares gradient magnitudes computed with standard `float64` vs `simd_f128` over many layers.

### JavaScript (`examples/js/`)

**`js_web_simulation.js`** — Node.js script that loads the WASM module and runs a physics simulation loop using 128-bit arithmetic, demonstrating the JS API.

### Rust (`examples/rust/`)

Rust crate under `examples/rust/` that calls the C FFI bindings and runs basic arithmetic ops from Rust.

---

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

Same thing in C++:

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
| [![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml) | Linux x86_64 | Scalar, SSE2, AVX2 | `ubuntu-latest` |
| [![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml) | Linux RISC-V64 | Scalar | `ubuntu-latest` + QEMU |
| [![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml) | Python Bindings | CPython Extension | `ubuntu-latest` |
| [![macOS](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml) | Apple Silicon (M1/M2/M3) | NEON | `macos-latest` |
| [![Windows](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml) | Windows x64 (MSVC) | Scalar, AVX2 | `windows-latest` |
| [![WASM](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml) | WebAssembly (Node.js) | WASM-SIMD, Scalar | `ubuntu-latest` + Emscripten |
| [![Mobile](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml) | Android ARM64, ARMv7 | NEON, Scalar | `ubuntu-latest` + QEMU |
| [![Rust](https://github.com/tiw302/simd-f128/actions/workflows/rust.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions/workflows/rust.yml) | Rust Bindings | Rust FFI (Linux, macOS, Win) | `ubuntu`, `macos`, `windows` |

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
├── assets/images/        # logo and documentation media
├── benchmarks/           # performance benchmarks (Google Benchmark & native)
│   ├── CMakeLists.txt
│   ├── bench_arithmetic.cpp
│   ├── bench_math.cpp
│   ├── bench_matrix.cpp
│   └── bench_compare.c
├── examples/             # runnable usage examples
│   ├── CMakeLists.txt
│   ├── c/                # C examples
│   ├── cpp/              # C++ examples
│   ├── js/               # JS/web examples
│   ├── python/           # Python examples
│   └── rust/             # Rust examples
├── tests/                # unit tests
│   ├── CMakeLists.txt
│   ├── c/                # C tests
│   ├── cpp/              # C++ tests
│   ├── js/               # JS tests
│   └── python/           # Python tests
├── .github/workflows/    # CI pipelines (linux, macos, windows, wasm, mobile)
├── include/              # core library headers
│   ├── simd_f128.h           # double-double arithmetic engine
│   ├── simd_f128_consts.h    # high-precision mathematical constants
│   ├── simd_f128_io.h        # string conversion and console output
│   ├── simd_f128_math.h      # math functions (exp, log, trig, hyperbolic, etc.)
│   ├── simd_f128_utils.h     # comparison and utility functions
│   ├── simd_f128_matrix.h    # mat2/mat3/mat4 and vec2/vec3/vec4 types
│   ├── simd_f128_random.h    # xoshiro256** PRNG producing simd_f128 uniform randoms
│   ├── simd_f128_array.h     # batch array operations
│   ├── simd_f128_vector.h    # vectorized 4-lane ops (simd_f128x4)
│   ├── simd_f128.hpp         # modern C++ wrapper with operator overloading
│   ├── simd_f128_complex.h   # complex number arithmetic
│   ├── simd_f128_complex.hpp # std::complex interoperability
│   └── simd_f128_eigen.hpp   # Eigen matrix traits
├── js/                   # JavaScript bindings and WebAssembly module
├── python/               # Python bindings (pybind11)
├── rust/                 # Rust bindings (FFI via cc)
├── docs/                 # MkDocs documentation
│   ├── index.md
│   ├── api_reference.md
│   ├── math_theory.md
│   └── demo/             # live WebAssembly demo
├── build.sh              # Unix build script
├── build.bat             # Windows build script
├── CMakeLists.txt        # cross-platform build configuration
└── LICENSE               # MIT license
```

---

## Used By

| Project | Description |
|---|---|
| [mandelbrot-c](https://github.com/tiw302/mandelbrot-c) | Deep-zoom Mandelbrot renderer in C, using simd-f128 for 128-bit precision coordinates.<br><br><div align="center"><img src="https://raw.githubusercontent.com/tiw302/mandelbrot-c/master/assets/images/Mandelbrot-Screenshot.png" width="180">&nbsp;<img src="https://raw.githubusercontent.com/tiw302/mandelbrot-c/master/assets/images/julia-Screenshot.png" width="180">&nbsp;<img src="https://raw.githubusercontent.com/tiw302/mandelbrot-c/master/assets/images/Mandelbrot-Screenshot2.png" width="180">&nbsp;<img src="https://raw.githubusercontent.com/tiw302/mandelbrot-c/master/assets/images/julia-Screenshot2.png" width="180"></div> |

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
