<p align="center">
  <img src="assets/images/logo.webp" width="400" alt="simd-f128 Logo">
  <br>
  <b>High-performance cross-platform 128-bit arithmetic for SIMD applications.</b>
</p>

# simd-f128

[![Linux](https://github.com/tiw302/simd-f128/actions/workflows/linux.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![macOS](https://github.com/tiw302/simd-f128/actions/workflows/macos.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![Windows](https://github.com/tiw302/simd-f128/actions/workflows/windows.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![WASM](https://github.com/tiw302/simd-f128/actions/workflows/wasm.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)
[![Mobile](https://github.com/tiw302/simd-f128/actions/workflows/mobile.yml/badge.svg)](https://github.com/tiw302/simd-f128/actions)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/Language-C11-00599C.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))

---

## Table of Contents

| Introduction | Setup & Build | Components | Resources | Community |
|---|---|---|---|---|
| [Overview](#introduction) | [Requirements](#requirements) | [Core Engine](#simd_f128h-core) | [API Docs](#api-reference) | [CI Status](#platform-support--ci-status) |
| [Why?](#why-simd-f128) | [Toolchains](#verified-toolchains) | [Constants](#simd_f128_constsh) | [Math Theory](#double-double-arithmetic) | [Contributing](#contributing) |
| [Philosophy](#design-philosophy) | [Installation](#build-and-installation) | [I/O Utilities](#simd_f128_ioh) | [Examples](#examples) | [License](#license) |

---

## Introduction

**simd-f128** is a professional-grade, header-only C library for **128-bit (Double-Double)** floating-point arithmetic. It targets the precision gap between standard 64-bit IEEE 754 doubles and heavyweight arbitrary-precision libraries, providing approximately 31-32 decimal digits of accuracy with zero heap allocation overhead.

Designed for demanding workloads such as fractal rendering, physical simulations, and orbital mechanics — where double precision falls short but `libquadmath` or GMP would be excessive.

---

## Why simd-f128?

Standard 64-bit `double` provides approximately 15-16 significant decimal digits. For most applications this is sufficient. However, certain problems expose its limits:

- **Deep-zoom fractals** — at zoom levels beyond ~10^-14, coordinates collapse to the same double value and detail disappears entirely.
- **Long-running simulations** — rounding errors accumulate over millions of iterations, causing physical simulations to diverge from the true trajectory.
- **Ill-conditioned linear algebra** — problems with large condition numbers lose accuracy rapidly in standard double arithmetic.

The alternatives each have significant trade-offs:

| Option | Precision | Allocation | Portability | Complexity |
|---|---|---|---|---|
| `double` | ~15 digits | None | Universal | None |
| **simd-f128** | **~31 digits** | **None** | **Universal** | **Low** |
| `long double` | 18-19 digits (x87) | None | Compiler-dependent | Low |
| `__float128` (GCC) | ~33 digits | None | GCC/Clang only | Medium |
| GMP / MPFR | Arbitrary | Heap | Portable | High |

simd-f128 occupies the space between `double` and full arbitrary-precision libraries: it roughly doubles usable precision with no additional memory allocation and no external dependencies.

---

## Design Philosophy

The library is built around three constraints that were never relaxed during development:

**Zero allocation.** Every operation executes entirely in CPU registers. There are no calls to `malloc`, no temporary buffers, and no GC pressure. This makes simd-f128 suitable for use inside tight render loops, interrupt handlers, and embedded firmware where heap allocation is prohibited.

**No configuration required.** The correct SIMD backend — AVX2, NEON, WASM-SIMD, or scalar — is selected automatically at compile time based on the target architecture. Passing the wrong flag produces a compile error immediately rather than silently degrading precision at runtime.

**Standard C foundation.** The library is built entirely on IEEE 754 `double` arithmetic and C11 standard library functions. It does not rely on compiler extensions, non-standard intrinsics outside of guarded `#ifdef` blocks, or platform-specific ABI assumptions. The scalar fallback compiles and produces correct results on any C99-compliant toolchain.

---

## Requirements

| Component | Requirement |
|---|---|
| C Standard | C11 or later (C99 compatible for scalar path) |
| Compiler | GCC 4.9+, Clang 3.5+, MSVC 2019+, Emscripten 3.0+ |
| Math library | `-lm` required on Linux/UNIX (for `fma()`) |

---

## Verified Toolchains

The following toolchains are tested on every commit via CI. All others fall back to the scalar path and are expected to produce correct results.

| Toolchain | Version | Platform | Backend |
|---|---|---|---|
| GCC | 11+ | Linux x86_64 | Scalar, AVX2 |
| GCC (aarch64-linux-gnu) | 11+ | Linux ARM64 (QEMU) | NEON |
| GCC (arm-linux-gnueabihf) | 11+ | Linux ARMv7 (QEMU) | Scalar + VFPv4 |
| GCC (riscv64-linux-gnu) | 11+ | Linux RISC-V64 (QEMU) | Scalar |
| Clang | 14+ | macOS Apple Silicon | NEON |
| Clang | 14+ | macOS Intel | Scalar, AVX2 |
| MSVC | 2022 | Windows x64 | Scalar |
| Emscripten | 3.0+ | WASM (Node.js) | WASM-SIMD, Scalar |

---

## Build and Installation

simd-f128 is header-only. The simplest integration is copying the `.h` files directly into your project, then defining the implementation macro in exactly one translation unit:

```c
#define SIMD_F128_IMPLEMENTATION
#include "simd_f128.h"
#include "simd_f128_io.h"   /* optional */
```

All other translation units include the headers without the macro.

### CMake

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

## simd_f128.h (Core)

The central engine of the library. Implements the Double-Double type and all fundamental arithmetic operations. All functions are `static inline` - no separate compilation unit is needed beyond the `SIMD_F128_IMPLEMENTATION` guard.

**Key properties:**

- **~106-bit mantissa** - roughly 31-32 decimal digits of precision.
- **Zero heap allocation** - all operations execute directly in CPU registers, suitable for tight inner loops.
- **Automatic SIMD dispatch** - selects AVX2 (`__m128d`) on Intel/AMD, NEON (`float64x2_t`) on ARM64/Apple Silicon, WASM-SIMD (`v128_t`) on the web, or falls back to scalar C99.
- **Branchless implementation** - consistent execution time, no pipeline stalls.
- **Strict IEEE 754 foundation** - built on standard `double`, fully compatible with existing hardware.

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

## simd_f128_consts.h

Pre-computed, high-precision mathematical constants stored as Double-Double pairs. Each constant captures the full ~106-bit mantissa, avoiding the precision loss inherent in standard 64-bit initialisers.

```c
#include "simd_f128.h"
#include "simd_f128_consts.h"

int main() {
    simd_f128 pi     = SIMD_F128_PI;    /* 3.14159265358979323846... */
    simd_f128 e      = SIMD_F128_E;     /* 2.71828182845904523536... */
    simd_f128 sqrt_2 = SIMD_F128_SQRT2; /* 1.41421356237309504880... */
    simd_f128 ln2    = SIMD_F128_LN2;   /* 0.69314718055994530941... */

    return 0;
}
```

---

## simd_f128_io.h

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

simd-f128 represents a value $x$ as the unevaluated sum of two IEEE 754 doubles:

$$x = x_{hi} + x_{lo}, \quad |x_{lo}| \leq \frac{1}{2} \, \text{ulp}(x_{hi})$$

This non-overlapping constraint provides ~106 bits of mantissa — approximately double the precision of a single `double`.

**Implementation basis:**

- **TwoSum (Knuth)** — error-free transformation for addition, capturing the exact rounding residual.
- **TwoProd (Dekker)** — error-free transformation for multiplication, exploiting FMA where available.
- **Normalisation** — each operation re-establishes the non-overlapping property before returning.

No memory allocation is required. The entire number lives in two registers.

**Known limitations:**

- Operations available: `add`, `sub`, `mul` only. Division, `sqrt`, and transcendental functions are not implemented and must be built on top of the provided primitives.
- Numerical range is identical to IEEE 754 `double` (~1.8 x 10^308). The library extends mantissa precision only.
- `NaN` and `Infinity` propagate through standard `double` rules.
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

## Project Structure

```text
.
├── assets/images/        # Logo and documentation media
├── examples/             # Runnable usage examples
│   ├── basic_arithmetic.c
│   ├── precision_demo.c
│   └── mandelbrot_core.c
├── tests/                # Arithmetic unit tests
│   └── test_arithmetic.c
├── .github/workflows/    # CI pipelines (linux, macos, windows, wasm, mobile)
├── simd_f128.h           # Core library - Double-Double arithmetic engine
├── simd_f128_consts.h    # High-precision mathematical constants
├── simd_f128_io.h        # String conversion and console output
├── CMakeLists.txt        # Cross-platform build configuration
└── LICENSE               # MIT License
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

This project is licensed under the [MIT License](LICENSE) - see the [LICENSE](LICENSE) file for details.
