# simd-f128 Documentation

Welcome to the official documentation for **simd-f128**.

The `simd-f128` library provides a professional-grade, header-only C implementation of 128-bit (Double-Double) floating-point arithmetic. By leveraging SIMD (Single Instruction, Multiple Data) intrinsics, it achieves approximately 31-32 decimal digits of precision with zero heap allocation overhead.

## Architecture & Features

- **Hardware Acceleration**: Auto-detects the optimal SIMD backend at compile time, seamlessly dispatching to **AVX2**, **SSE2**, or **NEON**.
- **WebAssembly Ready**: Ships with dual WASM modules (SIMD128 and Scalar) bringing backend-level precision to web browsers.
- **Python Bindings**: Distributed as a native C++ extension via PyBind11 for seamless data science integration.
- **Scientific Integrations**: Full `std::complex` interoperability and `Eigen` library matrix traits out of the box via `simd_f128_eigen.hpp`.
- **Robust Fallbacks**: Standard C11 fallback available for all unsupported architectures (RISC-V, ARMv7).

## Quick Links

- [GitHub Repository](https://github.com/tiw302/simd-f128)
- [Components Guide](components.md)
- [API Reference](api_reference.md)
- [Math Theory & Architecture](math_theory.md)
- [Live WebAssembly Demo](demo/index.html)

## Installation

### C/C++ (Header Only)

The simplest integration is copying the `include/` directory into your project. In exactly **one** C/C++ file, define the implementation macro before including:

```c
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.h>
```

**System Install via CMake (Recommended for linking)**

```bash
git clone https://github.com/tiw302/simd-f128.git
cd simd-f128
cmake -S . -B build
sudo cmake --install build
```

Then in your project's `CMakeLists.txt`:

```cmake
find_package(simd_fp REQUIRED)
target_link_libraries(your_target PRIVATE simd_fp::simd_fp)
```

### Python

The Python extension is available as a compiled wheel. It supports Python 3.7+ across Linux, macOS, and Windows.

```bash
pip install simd-f128
```

### Node.js & Web

Install the Emscripten-compiled WebAssembly module via NPM.

```bash
npm install @tiw302/simd-f128
```

## Quick Start Example (C++)

```cpp
#define SIMD_F128_IMPLEMENTATION
#include <simd_f128.hpp>
#include <iostream>

using namespace f128;

int main() {
    // standard double drops precision when adding extremely small values
    float128 a(1.0);
    float128 b(1e-17);

    float128 result = a + b;

    // float128 overloads std::ostream directly!
    std::cout << "result: " << result << "\n";

    return 0;
}
```

## Performance & Benchmarking

To run the benchmarks locally:

```bash
./build.sh bench
# or manually:
cmake -S . -B build_bench -DSIMD_F128_BUILD_BENCHMARKS=ON
cmake --build build_bench
./build_bench/benchmarks/bench_compare
./build_bench/benchmarks/bench_arithmetic
```


