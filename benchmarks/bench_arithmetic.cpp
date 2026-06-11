#include <benchmark/benchmark.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_vector.h"

/*
 * ---------------------------------------------------------
 * baseline: native 64-bit double precision
 * ---------------------------------------------------------
 */
static void BM_Double_Add(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
    double a = 1.0000001;
    double b = 2.0000002;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a += b);
    }
}
BENCHMARK(BM_Double_Add);

static void BM_Double_Mul(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
    double a = 1.0000001;
    double b = 2.0000002;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a *= b);
    }
}
BENCHMARK(BM_Double_Mul);

/*
 * ---------------------------------------------------------
 * baseline: gcc/clang 128-bit quad precision (if available)
 * ---------------------------------------------------------
 */
#ifdef __SIZEOF_FLOAT128__
static void BM_Float128_Add(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
    __float128 a = 1.0000001q;
    __float128 b = 2.0000002q;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a += b);
    }
}
BENCHMARK(BM_Float128_Add);

static void BM_Float128_Mul(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
    __float128 a = 1.0000001q;
    __float128 b = 2.0000002q;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a *= b);
    }
}
BENCHMARK(BM_Float128_Mul);
#endif

/*
 * ---------------------------------------------------------
 * simd-f128: 128-bit double-double precision
 * ---------------------------------------------------------
 */
static void BM_SimdF128_Add(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
    simd_f128 a = simd_f128_from_double(1.0000001);
    simd_f128 b = simd_f128_from_double(2.0000002);
    for (auto _ : state) {
        a = simd_f128_add(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_SimdF128_Add);

static void BM_SimdF128_Mul(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
    simd_f128 a = simd_f128_from_double(1.0000001);
    simd_f128 b = simd_f128_from_double(2.0000002);
    for (auto _ : state) {
        a = simd_f128_mul(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_SimdF128_Mul);

/*
 * ---------------------------------------------------------
 * simd-f128x4: AVX2 4-way parallel double-double precision
 * ---------------------------------------------------------
 */
#if defined(SIMD_F128_USE_AVX2)
static void BM_SimdF128x4_Add(benchmark::State& state) {
    // note: this measures latency due to the loop-carried dependency chain
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
    // note: this measures latency due to the loop-carried dependency chain
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
    // note: this measures latency due to the loop-carried dependency chain
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
#endif

BENCHMARK_MAIN();
