/* test_mpfr_accuracy.cpp
 *
 * tests mathematical correctness of transcendental functions using mpfr
 * as the ground truth. calculates ulp (unit in the last place) error.
 *
 * updated 2026-08-16
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <mpfr.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#define SIMD_F128_IMPLEMENTATION
#include "../../include/simd_f128.hpp"

using namespace f128;

// maximum acceptable ulp error
const double MAX_ULP = 16.0;

// helper: calculate ulp difference between f128 and mpfr
double calculate_ulp(float128 approx, mpfr_t exact) {
    // extract double-double components
    double hi, lo;
    simd_f128_extract(approx.data, &hi, &lo);

    // edge cases: nan, inf
    if (std::isnan(hi) && mpfr_nan_p(exact)) return 0.0;
    if (std::isinf(hi) && mpfr_inf_p(exact)) {
        if ((hi > 0) == (mpfr_sgn(exact) > 0)) return 0.0;
    }
    if (std::isnan(hi) || std::isinf(hi)) return INFINITY;  // unexpected edge case

    // convert float128 to mpfr
    mpfr_t approx_mp;
    mpfr_init2(approx_mp, 113);  // 113 bits for quad precision roughly
    mpfr_set_d(approx_mp, hi, MPFR_RNDN);

    mpfr_t lo_mp;
    mpfr_init2(lo_mp, 113);
    mpfr_set_d(lo_mp, lo, MPFR_RNDN);

    mpfr_add(approx_mp, approx_mp, lo_mp, MPFR_RNDN);

    // calculate diff = |approx - exact|
    mpfr_t diff, ulp_mp;
    mpfr_inits2(113, diff, ulp_mp, NULL);
    mpfr_sub(diff, approx_mp, exact, MPFR_RNDN);
    mpfr_abs(diff, diff, MPFR_RNDN);

    // compute 1 ulp at exact = 2^(exponent - 105) for double-double
    // approximate ulp for double-double is |exact| * 2^-105
    mpfr_abs(ulp_mp, exact, MPFR_RNDN);
    mpfr_mul_2si(ulp_mp, ulp_mp, -105, MPFR_RNDN);

    double ulp_error = 0.0;
    if (mpfr_zero_p(ulp_mp)) {
        if (mpfr_zero_p(diff))
            ulp_error = 0.0;
        else
            ulp_error = INFINITY;
    } else {
        mpfr_div(diff, diff, ulp_mp, MPFR_RNDN);
        ulp_error = mpfr_get_d(diff, MPFR_RNDN);
    }

    mpfr_clears(approx_mp, lo_mp, diff, ulp_mp, NULL);
    return ulp_error;
}

void test_function(const char* name, float128 (*func)(float128),
                   int (*mpfr_func)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t),
                   const std::vector<double>& test_points) {
    std::cout << "testing " << name << " against mpfr...\n";
    double max_err = 0.0;
    int failures = 0;

    mpfr_t in_mp, exact_mp;
    mpfr_inits2(113, in_mp, exact_mp, NULL);

    for (double x : test_points) {
        float128 input(x);
        float128 approx = func(input);

        mpfr_set_d(in_mp, x, MPFR_RNDN);
        mpfr_func(exact_mp, in_mp, MPFR_RNDN);

        double ulp = calculate_ulp(approx, exact_mp);
        if (ulp > max_err && ulp != INFINITY) {
            max_err = ulp;
        }

        if (ulp > MAX_ULP) {
            std::cerr << "  FAIL: " << name << "(" << x << ")\n"
                      << "    approx: " << approx << "\n"
                      << "    exact (mpfr): ";
            mpfr_out_str(stderr, 10, 35, exact_mp, MPFR_RNDN);
            std::cerr << "\n    ulp error: " << ulp << " > " << MAX_ULP << "\n";
            failures++;
        }
    }

    mpfr_clears(in_mp, exact_mp, NULL);

    if (failures == 0) {
        std::cout << "  ok. max ulp error: " << max_err << "\n";
    } else {
        std::cout << "  failed " << failures << " tests.\n";
        exit(1);
    }
}

int main() {
    std::cout << "running mpfr accuracy tests...\n";

    // test points: normal, near zero, large, edge cases
    std::vector<double> points = {0.0,
                                  1e-10,
                                  0.5,
                                  1.0,
                                  2.0,
                                  10.0,
                                  100.0,
                                  -1e-10,
                                  -0.5,
                                  -1.0,
                                  -2.0,
                                  -10.0,
                                  -100.0,
                                  std::numeric_limits<double>::infinity(),
                                  -std::numeric_limits<double>::infinity(),
                                  std::numeric_limits<double>::quiet_NaN()};

    std::vector<double> pos_points;
    for (double x : points)
        if (x >= 0.0 || std::isnan(x)) pos_points.push_back(x);

    test_function("sin", sin, mpfr_sin, points);
    test_function("cos", cos, mpfr_cos, points);
    test_function("exp", exp, mpfr_exp, points);
    test_function("sqrt", sqrt, mpfr_sqrt, pos_points);

    // remove 0 from log test points to avoid expected pole error spam unless handled
    std::vector<double> log_points;
    for (double x : pos_points)
        if (x != 0.0) log_points.push_back(x);
    test_function("log", log, mpfr_log, log_points);

    std::cout << "all mpfr tests passed.\n";
    return 0;
}
