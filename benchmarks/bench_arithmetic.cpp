/* bench_arithmetic.cpp
 *
 * performance benchmarks comparing simd-f128 against 64-bit double. */

#include <benchmark/benchmark.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_vector.h"
#include "../include/simd_f128_array.h"

// ██████   ██████  ██    ██ ██████  ██      ███████
// ██   ██ ██    ██ ██    ██ ██   ██ ██      ██
// ██   ██ ██    ██ ██    ██ ██████  ██      █████
// ██   ██ ██    ██ ██    ██ ██   ██ ██      ██
// ██████   ██████   ██████  ██████  ███████ ███████
//
// >>baseline: native 64-bit double precision
static void BM_Double_Add(benchmark::State& state) {
    double a = 1.0000001;
    double b = 2.0000002;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a += b);
    }
}
BENCHMARK(BM_Double_Add);

static void BM_Double_Mul(benchmark::State& state) {
    double a = 1.0000001;
    double b = 2.0000002;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a *= b);
    }
}
BENCHMARK(BM_Double_Mul);

//  ██████  ██    ██  █████  ██████
// ██    ██ ██    ██ ██   ██ ██   ██
// ██    ██ ██    ██ ███████ ██   ██
// ██  █ ██ ██    ██ ██   ██ ██   ██
//  █████ █  ██████  ██   ██ ██████
//
// >>baseline: gcc/clang 128-bit quad precision
#ifdef __SIZEOF_FLOAT128__

static void BM_Float128_Add(benchmark::State& state) {
    __float128 a = 1.0000001q;
    __float128 b = 2.0000002q;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a += b);
    }
}
BENCHMARK(BM_Float128_Add);

static void BM_Float128_Mul(benchmark::State& state) {
    __float128 a = 1.0000001q;
    __float128 b = 2.0000002q;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a *= b);
    }
}
BENCHMARK(BM_Float128_Mul);
#endif // __SIZEOF_FLOAT128__

// ███████  ██  █████   █████
// ██      ███ ██   ██ ██   ██
// █████    ██   ███    █████
// ██       ██  ██     ██   ██
// ██       ██ ███████  █████
//
// >>simd-f128: 128-bit double-double precision
static void BM_SimdF128_Add(benchmark::State& state) {
    simd_f128 a = simd_f128_from_double(1.0000001);
    simd_f128 b = simd_f128_from_double(2.0000002);
    for (auto _ : state) {
        a = simd_f128_add(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_SimdF128_Add);

static void BM_SimdF128_Mul(benchmark::State& state) {
    simd_f128 a = simd_f128_from_double(1.0000001);
    simd_f128 b = simd_f128_from_double(2.0000002);
    for (auto _ : state) {
        a = simd_f128_mul(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_SimdF128_Mul);

//  █████  ██    ██ ██   ██  █████
// ██   ██ ██    ██  ██ ██  ██   ██
// ███████ ██    ██   ███     ███
// ██   ██  ██  ██   ██ ██   ██
// ██   ██   ████   ██   ██ ███████
//
// >>simd-f128x4: avx2 4-way parallel double-double precision
#if defined(SIMD_F128_USE_AVX2)

static void BM_SimdF128x4_Add(benchmark::State& state) {
    simd_f128x4 a = simd_f128x4_from_doubles(1.0, 2.0, 3.0, 4.0);
    simd_f128x4 b = simd_f128x4_from_doubles(5.0, 6.0, 7.0, 8.0);
    for (auto _ : state) {
        a = simd_f128x4_add(a, b);
        benchmark::DoNotOptimize(a);
    }
    state.SetItemsProcessed(state.iterations() * 4);
}
BENCHMARK(BM_SimdF128x4_Add);

static void BM_SimdF128x4_Mul(benchmark::State& state) {
    simd_f128x4 a = simd_f128x4_from_doubles(1.0, 2.0, 3.0, 4.0);
    simd_f128x4 b = simd_f128x4_from_doubles(5.0, 6.0, 7.0, 8.0);
    for (auto _ : state) {
        a = simd_f128x4_mul(a, b);
        benchmark::DoNotOptimize(a);
    }
    state.SetItemsProcessed(state.iterations() * 4);
}
BENCHMARK(BM_SimdF128x4_Mul);

static void BM_SimdF128x4_Sub(benchmark::State& state) {
    simd_f128x4 a = simd_f128x4_from_doubles(1.0, 2.0, 3.0, 4.0);
    simd_f128x4 b = simd_f128x4_from_doubles(5.0, 6.0, 7.0, 8.0);
    for (auto _ : state) {
        a = simd_f128x4_sub(a, b);
        benchmark::DoNotOptimize(a);
    }
    state.SetItemsProcessed(state.iterations() * 4);
}
BENCHMARK(BM_SimdF128x4_Sub);

static void BM_SimdF128x4_Div(benchmark::State& state) {
    simd_f128x4 a = simd_f128x4_from_doubles(3.14, 3.14, 3.14, 3.14);
    simd_f128x4 b = simd_f128x4_from_doubles(2.71, 2.71, 2.71, 2.71);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
        simd_f128x4 r = simd_f128x4_div(a, b);
        benchmark::DoNotOptimize(r);
    }
    state.SetItemsProcessed(state.iterations() * 4);
}
BENCHMARK(BM_SimdF128x4_Div);

static void BM_SimdF128x4_Sqrt(benchmark::State& state) {
    simd_f128x4 a = simd_f128x4_from_doubles(2.0, 3.0, 4.0, 5.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a);
        simd_f128x4 r = simd_f128x4_sqrt(a);
        benchmark::DoNotOptimize(r);
    }
    state.SetItemsProcessed(state.iterations() * 4);
}
BENCHMARK(BM_SimdF128x4_Sqrt);
#endif // SIMD_F128_USE_AVX2

//  █████  ██████  ██████   █████  ██    ██
// ██   ██ ██   ██ ██   ██ ██   ██  ██  ██ 
// ███████ ██████  ██████  ███████   ████  
// ██   ██ ██   ██ ██   ██ ██   ██    ██   
// ██   ██ ██   ██ ██   ██ ██   ██    ██   
//
// >>simd-f128 array processing: soa vs aos
#define SOA_BENCH_LEN 1024
static void BM_SimdF128_Array_SoA_Add(benchmark::State& state) {
    double a_hi[SOA_BENCH_LEN], a_lo[SOA_BENCH_LEN];
    double b_hi[SOA_BENCH_LEN], b_lo[SOA_BENCH_LEN];
    double out_hi[SOA_BENCH_LEN], out_lo[SOA_BENCH_LEN];
    for (int i = 0; i < SOA_BENCH_LEN; i++) {
        a_hi[i] = 1.0; a_lo[i] = 1e-16;
        b_hi[i] = 2.0; b_lo[i] = 1e-16;
    }
    for (auto _ : state) {
        simd_f128_array_add_soa(a_hi, a_lo, b_hi, b_lo, out_hi, out_lo, SOA_BENCH_LEN);
        benchmark::DoNotOptimize(out_hi);
        benchmark::DoNotOptimize(out_lo);
    }
    state.SetItemsProcessed(state.iterations() * SOA_BENCH_LEN);
}
BENCHMARK(BM_SimdF128_Array_SoA_Add);

static void BM_SimdF128_Array_SoA_Mul(benchmark::State& state) {
    double a_hi[SOA_BENCH_LEN], a_lo[SOA_BENCH_LEN];
    double b_hi[SOA_BENCH_LEN], b_lo[SOA_BENCH_LEN];
    double out_hi[SOA_BENCH_LEN], out_lo[SOA_BENCH_LEN];
    for (int i = 0; i < SOA_BENCH_LEN; i++) {
        a_hi[i] = 1.0; a_lo[i] = 1e-16;
        b_hi[i] = 2.0; b_lo[i] = 1e-16;
    }
    for (auto _ : state) {
        simd_f128_array_mul_soa(a_hi, a_lo, b_hi, b_lo, out_hi, out_lo, SOA_BENCH_LEN);
        benchmark::DoNotOptimize(out_hi);
        benchmark::DoNotOptimize(out_lo);
    }
    state.SetItemsProcessed(state.iterations() * SOA_BENCH_LEN);
}
BENCHMARK(BM_SimdF128_Array_SoA_Mul);

BENCHMARK_MAIN();
