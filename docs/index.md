# simd-f128 Documentation

Welcome to the official documentation for **simd-f128**.

The `simd-f128` library provides a high-performance, cross-platform implementation of 128-bit (double-double) floating-point arithmetic. By leveraging SIMD (Single Instruction, Multiple Data) intrinsics, it achieves mathematical precision exceeding standard 64-bit IEEE-754 doubles while maintaining exceptionally low computational overhead.

## Architecture & Features

- **Hardware Acceleration**: Native support for AVX2 and SSE2 instruction sets, allowing for auto-vectorization in tight mathematical loops.
- **WebAssembly Ready**: Compiles natively to WASM with SIMD128 support, bringing backend-level precision to web browsers.
- **Python Bindings**: Distributed as a native C++ extension via PyBind11 for seamless data science integration.
- **Scientific Integrations**: Full `std::complex` interoperability and `Eigen` library matrix traits out of the box.
- **Robust Fallbacks**: Utilizes exact arithmetic algorithms (like Dekker's Split) to ensure precision on embedded platforms lacking FMA (Fused Multiply-Add).

## Quick Links

- [GitHub Repository](https://github.com/tiw302/simd-f128)
- [Live 128-bit Web Demo](https://tiw302.github.io/simd-f128/demo.html)

## Installation

### C/C++ (Header Only)

The library is designed to be easily integrated into any C or C++ project without complex build systems.

1. Drop the `simd_f128.h` and `simd_f128.hpp` files into your project's include path.
2. In exactly **one** C/C++ file, define the implementation macro before including:

```c
#define SIMD_F128_IMPLEMENTATION
#include "simd_f128.h"
```

### Python

The Python extension is available as a compiled wheel. It supports Python 3.7+ across Linux, macOS, and Windows.

```bash
pip install simd-f128
```

### Node.js & Web

Install the Emscripten-compiled WebAssembly module via NPM.

```bash
npm install simd-f128
```

## Quick Start Example (C++)

```cpp
#include <iostream>
#include "simd_f128.hpp"
#include "simd_f128_io.h"

using namespace f128;

int main() {
    // standard double drops precision when adding extremely small values
    float128 a = float128(1.0);
    float128 b = float128(1e-17);
    
    float128 result = a + b;
    
    char buffer[128];
    simd_f128_to_string(buffer, sizeof(buffer), result.data);
    std::cout << "result: " << buffer << std::endl;
    
    return 0;
}
```

## Performance & Benchmarking

The library uses Google Benchmark for rigorous performance profiling. On modern x86 architecture, `simd-f128` achieves approximately 30% higher throughput compared to the GCC native software quad-precision `__float128`.

To run the benchmarks locally:

```bash
cmake -S . -B build -DSIMD_F128_BUILD_BENCHMARKS=ON
cmake --build build
./build/bench_arithmetic
```
