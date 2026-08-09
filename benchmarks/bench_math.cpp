/* bench_math.cpp
 *
 * performance benchmarks for extended math functions (exp, log, trig). */

#include <benchmark/benchmark.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_consts.h"
#include "../include/simd_f128_io.h"
#include "../include/simd_f128_math.h"
#include "../include/simd_f128_utils.h"

//  ██████  ██████  ██████  ███████
// ██      ██    ██ ██   ██ ██
// ██      ██    ██ ██████  █████
// ██      ██    ██ ██   ██ ██
//  ██████  ██████  ██   ██ ███████
//
// >>core arithmetic
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
    simd_f128 b = simd_f128_from_double(1.0000002);
    for (auto _ : state) {
        a = simd_f128_mul(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_SimdF128_Mul);

static void BM_SimdF128_Div(benchmark::State& state) {
    simd_f128 a = simd_f128_from_double(3.14159);
    simd_f128 b = simd_f128_from_double(2.71828);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
        simd_f128 r = simd_f128_div(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_SimdF128_Div);

static void BM_SimdF128_Sqrt(benchmark::State& state) {
    simd_f128 a = simd_f128_from_double(2.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a);
        simd_f128 r = simd_f128_sqrt(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_SimdF128_Sqrt);

// ███    ███  █████  ████████ ██   ██
// ████  ████ ██   ██    ██    ██   ██
// ██ ████ ██ ███████    ██    ███████
// ██  ██  ██ ██   ██    ██    ██   ██
// ██      ██ ██   ██    ██    ██   ██
//
// >>math functions
static void BM_SimdF128_Exp(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(0.5);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_exp(x));
    }
}
BENCHMARK(BM_SimdF128_Exp);

static void BM_SimdF128_Log(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(2.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_log(x));
    }
}
BENCHMARK(BM_SimdF128_Log);

static void BM_SimdF128_Sin(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(1.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_sin(x));
    }
}
BENCHMARK(BM_SimdF128_Sin);

static void BM_SimdF128_Cos(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(1.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_cos(x));
    }
}
BENCHMARK(BM_SimdF128_Cos);

static void BM_SimdF128_Atan(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(0.5);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_atan(x));
    }
}
BENCHMARK(BM_SimdF128_Atan);

static void BM_SimdF128_Pow(benchmark::State& state) {
    simd_f128 b = simd_f128_from_double(2.0);
    simd_f128 e = simd_f128_from_double(10.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_pow(b, e));
    }
}
BENCHMARK(BM_SimdF128_Pow);

// ███    ██ ███████ ██     ██
// ████   ██ ██      ██     ██
// ██ ██  ██ █████   ██  █  ██
// ██  ██ ██ ██      ██ ███ ██
// ██   ████ ███████  ███ ███
//
// >>new math functions
static void BM_SimdF128_Tan(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(1.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_tan(x));
    }
}
BENCHMARK(BM_SimdF128_Tan);

static void BM_SimdF128_Sinh(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(1.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_sinh(x));
    }
}
BENCHMARK(BM_SimdF128_Sinh);

static void BM_SimdF128_Cosh(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(1.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_cosh(x));
    }
}
BENCHMARK(BM_SimdF128_Cosh);

static void BM_SimdF128_Tanh(benchmark::State& state) {
    simd_f128 x = simd_f128_from_double(1.0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_tanh(x));
    }
}
BENCHMARK(BM_SimdF128_Tanh);

// ██  ██████
// ██ ██    ██
// ██ ██    ██
// ██ ██    ██
// ██  ██████
//
// >>string formatting and parsing
static void BM_SimdF128_ToString(benchmark::State& state) {
    simd_f128 x = SIMD_F128_PI;
    char buf[128];
    for (auto _ : state) {
        simd_f128_to_string(buf, sizeof(buf), x);
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_SimdF128_ToString);

static void BM_SimdF128_FromString(benchmark::State& state) {
    const char* str = "3.1415926535897932384626433832795";
    for (auto _ : state) {
        benchmark::DoNotOptimize(simd_f128_from_string(str));
    }
}
BENCHMARK(BM_SimdF128_FromString);

BENCHMARK_MAIN();
