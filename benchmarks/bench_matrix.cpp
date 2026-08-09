/* bench_matrix.cpp
 *
 * performance benchmarks for 4d vectors and 4x4 matrices. */

#include <benchmark/benchmark.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_matrix.h"

// ███    ███  █████  ████████ ██████  ██ ██   ██
// ████  ████ ██   ██    ██    ██   ██ ██  ██ ██
// ██ ████ ██ ███████    ██    ██████  ██   ███
// ██  ██  ██ ██   ██    ██    ██   ██ ██  ██ ██
// ██      ██ ██   ██    ██    ██   ██ ██ ██   ██
//
// >>matrix-vector multiplication (4x4 * 4x1)
static void BM_Double_Mat4_Mul_Vec4(benchmark::State& state) {
    double a[16] = {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 1.0, 2.0, 3.0,
        4.0, 5.0, 6.0, 7.0
    };
    double v[4] = {1.1, 2.2, 3.3, 4.4};
    double out[4] = {0};

    for (auto _ : state) {
        for (int i = 0; i < 4; ++i) {
            double sum = 0.0;
            for (int k = 0; k < 4; ++k) {
                sum += a[i*4+k] * v[k];
            }
            out[i] = sum;
        }
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(BM_Double_Mat4_Mul_Vec4);

static void BM_SimdF128_Mat4_Mul_Vec4(benchmark::State& state) {
    simd_f128_mat4 a = simd_f128_mat4_identity();
    simd_f128_vec4 v = {
        simd_f128_from_double(1.1),
        simd_f128_from_double(2.2),
        simd_f128_from_double(3.3),
        simd_f128_from_double(4.4)
    };
    for (auto _ : state) {
        simd_f128_vec4 r = simd_f128_mat4_mul_vec4(a, v);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_SimdF128_Mat4_Mul_Vec4);

// ███    ███  █████  ████████ ██████  ██ ██   ██
// ████  ████ ██   ██    ██    ██   ██ ██  ██ ██
// ██ ████ ██ ███████    ██    ██████  ██   ███
// ██  ██  ██ ██   ██    ██    ██   ██ ██  ██ ██
// ██      ██ ██   ██    ██    ██   ██ ██ ██   ██
//
// >>matrix multiplication (4x4)
static void BM_Double_Mat4_Mul(benchmark::State& state) {
    double a[16] = {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 1.0, 2.0, 3.0,
        4.0, 5.0, 6.0, 7.0
    };
    double b[16] = {
        7.0, 6.0, 5.0, 4.0,
        3.0, 2.0, 1.0, 9.0,
        8.0, 7.0, 6.0, 5.0,
        4.0, 3.0, 2.0, 1.0
    };
    double c[16] = {0};

    for (auto _ : state) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double sum = 0.0;
                for (int k = 0; k < 4; ++k) {
                    sum += a[i*4+k] * b[k*4+j];
                }
                c[i*4+j] = sum;
            }
        }
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(BM_Double_Mat4_Mul);

static void BM_SimdF128_Mat4_Mul(benchmark::State& state) {
    simd_f128_mat4 a = simd_f128_mat4_identity();
    simd_f128_mat4 b = simd_f128_mat4_identity();

    // add some values to prevent trivial zeroing
    a.m[0][1] = simd_f128_from_double(2.0);
    b.m[1][0] = simd_f128_from_double(3.0);

    for (auto _ : state) {
        simd_f128_mat4 c = simd_f128_mat4_mul(a, b);
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(BM_SimdF128_Mat4_Mul);

BENCHMARK_MAIN();
