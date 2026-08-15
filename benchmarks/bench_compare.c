/* bench_compare.c
 *
 * manual timing comparison without external benchmark frameworks. */

#define SIMD_F128_IMPLEMENTATION
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/simd_f128.h"

#define ITERATIONS 10000000

#ifdef _WIN32
#include <windows.h>
static double get_time(void) {
    LARGE_INTEGER count, freq;
    QueryPerformanceCounter(&count);
    QueryPerformanceFrequency(&freq);
    return (double)count.QuadPart / (double)freq.QuadPart;
}
#else
static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}
#endif

/* volatile sinks are used here intentionally to prevent aggressive compiler
 * dead-code elimination (dce). without this, the compiler would see that the
 * calculation results are never used and would optimize the entire loop away to zero time. */
volatile double g_sink_double;
volatile long double g_sink_ldouble;

#ifdef __SIZEOF_FLOAT128__
volatile __float128 g_sink_float128;
#endif
void sink_simd(simd_f128 x) {
    double hi, lo;
    simd_f128_extract(x, &hi, &lo);
    g_sink_double = hi + lo;
}

void bench_double(void) {
    double a_add = 1.1, b_add = 0.0000001;
    double a_mul = 1.1, b_mul = 1.0000000001;
    double a_div = 1.1, b_div = 1.0000000001;

    double t0 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_add = a_add + b_add;
    }
    double t1 = get_time();
    g_sink_double = a_add;

    double t2 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_mul = a_mul * b_mul;
    }
    double t3 = get_time();
    g_sink_double = a_mul;

    double t4 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_div = a_div / b_div;
    }
    double t5 = get_time();
    g_sink_double = a_div;

    printf("| double (64-bit)    | %8.2f | %8.2f | %8.2f |\n", (t1 - t0) * 1000.0,
           (t3 - t2) * 1000.0, (t5 - t4) * 1000.0);
}

void bench_long_double(void) {
    long double a_add = 1.1L, b_add = 0.0000001L;
    long double a_mul = 1.1L, b_mul = 1.0000000001L;
    long double a_div = 1.1L, b_div = 1.0000000001L;

    double t0 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_add = a_add + b_add;
    }
    double t1 = get_time();
    g_sink_ldouble = a_add;

    double t2 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_mul = a_mul * b_mul;
    }
    double t3 = get_time();
    g_sink_ldouble = a_mul;

    double t4 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_div = a_div / b_div;
    }
    double t5 = get_time();
    g_sink_ldouble = a_div;

    printf("| long double (x87)  | %8.2f | %8.2f | %8.2f |\n", (t1 - t0) * 1000.0,
           (t3 - t2) * 1000.0, (t5 - t4) * 1000.0);
}

#ifdef __SIZEOF_FLOAT128__
void bench_float128(void) {
    __float128 a_add = 1.1q, b_add = 0.0000001q;
    __float128 a_mul = 1.1q, b_mul = 1.0000000001q;
    __float128 a_div = 1.1q, b_div = 1.0000000001q;

    double t0 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_add = a_add + b_add;
    }
    double t1 = get_time();
    g_sink_float128 = a_add;

    double t2 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_mul = a_mul * b_mul;
    }
    double t3 = get_time();
    g_sink_float128 = a_mul;

    double t4 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_div = a_div / b_div;
    }
    double t5 = get_time();
    g_sink_float128 = a_div;

    printf("| __float128 (GCC)   | %8.2f | %8.2f | %8.2f |\n", (t1 - t0) * 1000.0,
           (t3 - t2) * 1000.0, (t5 - t4) * 1000.0);
}
#endif

void bench_simd_f128(void) {
    simd_f128 a_add = simd_f128_from_double(1.1);
    simd_f128 b_add = simd_f128_from_double(0.0000001);

    simd_f128 a_mul = simd_f128_from_double(1.1);
    simd_f128 b_mul = simd_f128_from_double(1.0000000001);

    simd_f128 a_div = simd_f128_from_double(1.1);
    simd_f128 b_div = simd_f128_from_double(1.0000000001);

    double t0 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_add = simd_f128_add(a_add, b_add);
    }
    double t1 = get_time();
    sink_simd(a_add);

    double t2 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_mul = simd_f128_mul(a_mul, b_mul);
    }
    double t3 = get_time();
    sink_simd(a_mul);

    double t4 = get_time();
    for (int i = 0; i < ITERATIONS; ++i) {
        a_div = simd_f128_div(a_div, b_div);
    }
    double t5 = get_time();
    sink_simd(a_div);

    printf("| simd-f128 (SIMD)   | %8.2f | %8.2f | %8.2f |\n", (t1 - t0) * 1000.0,
           (t3 - t2) * 1000.0, (t5 - t4) * 1000.0);
}

int main(void) {
    printf("\n");
    printf("simd-f128 Manual Benchmark Comparison\n");
    printf("Iterations: %d operations per test (latency mode)\n\n", ITERATIONS);
    printf("| Data Type          | Add (ms) | Mul (ms) | Div (ms) |\n");
    printf("|--------------------|----------|----------|----------|\n");

    bench_double();
    bench_long_double();
#ifdef __SIZEOF_FLOAT128__
    bench_float128();
#endif
    bench_simd_f128();

    printf("\n");
    return 0;
}
