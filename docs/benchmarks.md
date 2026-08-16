# Performance & Benchmarks

simd-f128 is designed to out-perform software-emulated `__float128` routines by utilizing Double-Double arithmetic packed into hardware vector registers. 

## Accuracy and ULP Testing

Beyond speed, the library maintains strict correctness verified against the GNU MPFR library (Multiple Precision Floating-Point Reliable). Integration tests (`test_mpfr_accuracy.cpp`) assert that the unit in the last place (ULP) drift remains within acceptable boundaries (~4-16 ULP) for all transcendental functions (`sin`, `cos`, `exp`, `log`, `sqrt`) across edge cases and subnormals.

## Benchmark Methodology

All C/C++ benchmarks execute via Google Benchmark. Latency measurements test isolated operations through 10,000,000 iterations.
Flags used: `-O3 -march=native -ffast-math`.

Hardware: 12-core x86_64, 3.2GHz, AVX2 enabled.

## Latency Comparison

| Data Type | Add (ms) | Mul (ms) | Div (ms) | Relative Multiplication Speed |
|---|---|---|---|---|
| `double` (64-bit) | 8.80 | 8.07 | 33.06 | 1.00x (Baseline) |
| `long double` (x87) | 16.66 | 17.28 | 38.58 | 0.47x |
| `__float128` (GCC) | 152.31 | 189.81 | 276.00 | 0.04x |
| **simd-f128 (SIMD)** | **73.60** | **56.15** | **159.70** | **0.14x (3.38x faster than GCC)** |

![simd-f128 Benchmark Comparison](https://raw.githubusercontent.com/tiw302/simd-f128/master/assets/benchmarks/benchmark.png)

### Advanced Math Functions (Latency)

This chart illustrates the nanosecond latency for extended mathematical functions (`simd_f128_sin`, `simd_f128_exp`, etc.) processing 128-bit precision floats.

![simd-f128 Math Functions Latency](https://raw.githubusercontent.com/tiw302/simd-f128/master/assets/benchmarks/math_latency.png)

## WebAssembly Performance

WebAssembly execution is split into two variants. When running in a modern V8 environment (Node.js/Chrome) with WASM-SIMD128 enabled, the library maps Double-Double operations to native 128-bit WASM instructions (`v128`). 

| Environment | 1M Multiplications | Notes |
|---|---|---|
| **WASM-SIMD128** | ~310 ms | Standard behavior in modern V8 contexts. |
| **WASM-Scalar** | ~481 ms | Fallback for legacy environments lacking SIMD. |

## Reproducibility

To run the local C benchmark suite and verify the performance on target hardware:

```bash
mkdir build_bench && cd build_bench
cmake .. -DCMAKE_BUILD_TYPE=Release -DSIMD_F128_BUILD_BENCHMARKS=ON
cmake --build .

./benchmarks/bench_compare
./benchmarks/bench_arithmetic
```
