/* simd_f128_python.cpp -- high-performance 128-bit (double-double) arithmetic for python.
 * project url: https://github.com/tiw302/simd-f128
 *
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 *
 * python bindings:
 * ----------------
 * pybind11 wrapper for the c++ math core. maps operator overloads
 * and math functions directly to python without precision loss.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.hpp"
#include "../include/simd_f128_complex.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simd_f128, m) {
    m.doc() = "simd-f128: 128-bit double-double arithmetic module";

    // ============================================================================
    // class float128
    // ============================================================================
    py::class_<f128::float128>(m, "Float128")
        .def(py::init<>())
        .def(py::init<double>())
        // parse from string to avoid pybind11's 64-bit float truncation
        .def(py::init(
            [](const std::string& s) { return f128::float128(simd_f128_from_string(s.c_str())); }))
        .def("to_string", &f128::float128::to_string)
        .def("__str__", &f128::float128::to_string)
        // return eval-safe string for __repr__
        .def("__repr__",
             [](const f128::float128& self) { return "Float128('" + self.to_string() + "')"; })
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= py::self)
        .def(py::self /= py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(-py::self);

    // ============================================================================
    // class complex128
    // ============================================================================
    py::class_<f128::complex128>(m, "Complex128")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("real"), py::arg("imag") = 0.0)
        .def(py::init<f128::float128, f128::float128>(), py::arg("real"),
             py::arg("imag") = f128::float128(0.0))
        .def("real", &f128::complex128::real)
        .def("imag", &f128::complex128::imag)
        .def("to_string", &f128::complex128::to_string)
        .def("__str__", &f128::complex128::to_string)
        .def("__repr__", &f128::complex128::to_string)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= py::self)
        .def(py::self /= py::self);

    // ============================================================================
    // math functions
    // ============================================================================
    // bind math functions. use static_cast to resolve float/complex overloads.
    m.def("exp", &f128::exp);
    m.def("log", &f128::log);
    m.def("pow", &f128::pow);
    m.def("sin", static_cast<f128::float128 (*)(f128::float128)>(&f128::sin));
    m.def("sin", static_cast<f128::complex128 (*)(const f128::complex128&)>(&f128::sin));
    m.def("cos", static_cast<f128::float128 (*)(f128::float128)>(&f128::cos));
    m.def("cos", static_cast<f128::complex128 (*)(const f128::complex128&)>(&f128::cos));
    m.def("tan", static_cast<f128::float128 (*)(f128::float128)>(&f128::tan));
    m.def("tan", static_cast<f128::complex128 (*)(const f128::complex128&)>(&f128::tan));
    m.def("sinh", static_cast<f128::float128 (*)(f128::float128)>(&f128::sinh));
    m.def("sinh", static_cast<f128::complex128 (*)(const f128::complex128&)>(&f128::sinh));
    m.def("cosh", static_cast<f128::float128 (*)(f128::float128)>(&f128::cosh));
    m.def("cosh", static_cast<f128::complex128 (*)(const f128::complex128&)>(&f128::cosh));
    m.def("tanh", static_cast<f128::float128 (*)(f128::float128)>(&f128::tanh));
    m.def("tanh", static_cast<f128::complex128 (*)(const f128::complex128&)>(&f128::tanh));
    m.def("sqrt", &f128::sqrt);
    m.def("abs", static_cast<f128::float128 (*)(f128::float128)>(&f128::abs));
    m.def("abs", static_cast<f128::float128 (*)(const f128::complex128&)>(&f128::abs));
    m.def("floor", &f128::floor);
    m.def("ceil", &f128::ceil);
    m.def("trunc", &f128::trunc);
    m.def("round", &f128::round);
    m.def("fmod", &f128::fmod);
    m.def("atan", &f128::atan);
    m.def("atan2", &f128::atan2);
    m.def("asin", &f128::asin);
    m.def("acos", &f128::acos);
    m.def("isnan", &f128::isnan);
    m.def("isinf", &f128::isinf);
    m.def("abs_sqr", &f128::abs_sqr);

    // ============================================================================
    // implicit conversions
    // ============================================================================
    // allow implicit conversion from python's built-in complex type
    py::implicitly_convertible<std::complex<double>, f128::complex128>();

    // ============================================================================
    // constants
    // ============================================================================
    // math constants
    m.attr("pi") = f128::pi;
    m.attr("e") = f128::e;
    m.attr("sqrt2") = f128::sqrt2;
    m.attr("ln2") = f128::ln2;

    // ============================================================================
    // numpy vectorization
    // ============================================================================
    // vectorized numpy array operations for (n, 2) shapes
    m.def(
        "add_arrays",
        [](py::array_t<double> a, py::array_t<double> b) -> py::array_t<double> {
            py::buffer_info buf_a = a.request(), buf_b = b.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2 || buf_b.ndim != 2 || buf_b.shape[1] != 2)
                throw std::runtime_error("Inputs must have shape (N, 2)");
            if (buf_a.shape[0] != buf_b.shape[0])
                throw std::runtime_error("Input shapes must match");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_b = static_cast<double*>(buf_b.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sb = simd_f128_from_hi_lo(ptr_b[i * 2], ptr_b[i * 2 + 1]);
                simd_f128 sr = simd_f128_add(sa, sb);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise addition of (N, 2) arrays");

    m.def(
        "sub_arrays",
        [](py::array_t<double> a, py::array_t<double> b) -> py::array_t<double> {
            py::buffer_info buf_a = a.request(), buf_b = b.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2 || buf_b.ndim != 2 || buf_b.shape[1] != 2)
                throw std::runtime_error("Inputs must have shape (N, 2)");
            if (buf_a.shape[0] != buf_b.shape[0])
                throw std::runtime_error("Input shapes must match");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_b = static_cast<double*>(buf_b.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sb = simd_f128_from_hi_lo(ptr_b[i * 2], ptr_b[i * 2 + 1]);
                simd_f128 sr = simd_f128_sub(sa, sb);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise subtraction of (N, 2) arrays");

    m.def(
        "mul_arrays",
        [](py::array_t<double> a, py::array_t<double> b) -> py::array_t<double> {
            py::buffer_info buf_a = a.request(), buf_b = b.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2 || buf_b.ndim != 2 || buf_b.shape[1] != 2)
                throw std::runtime_error("Inputs must have shape (N, 2)");
            if (buf_a.shape[0] != buf_b.shape[0])
                throw std::runtime_error("Input shapes must match");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_b = static_cast<double*>(buf_b.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sb = simd_f128_from_hi_lo(ptr_b[i * 2], ptr_b[i * 2 + 1]);
                simd_f128 sr = simd_f128_mul(sa, sb);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise multiplication of (N, 2) arrays");

    m.def(
        "div_arrays",
        [](py::array_t<double> a, py::array_t<double> b) -> py::array_t<double> {
            py::buffer_info buf_a = a.request(), buf_b = b.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2 || buf_b.ndim != 2 || buf_b.shape[1] != 2)
                throw std::runtime_error("Inputs must have shape (N, 2)");
            if (buf_a.shape[0] != buf_b.shape[0])
                throw std::runtime_error("Input shapes must match");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_b = static_cast<double*>(buf_b.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sb = simd_f128_from_hi_lo(ptr_b[i * 2], ptr_b[i * 2 + 1]);
                simd_f128 sr = simd_f128_div(sa, sb);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise division of (N, 2) arrays");

    m.def(
        "sin_arrays",
        [](py::array_t<double> a) -> py::array_t<double> {
            py::buffer_info buf_a = a.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2)
                throw std::runtime_error("Input must have shape (N, 2)");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sr = simd_f128_sin(sa);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise sin of (N, 2) arrays");

    m.def(
        "cos_arrays",
        [](py::array_t<double> a) -> py::array_t<double> {
            py::buffer_info buf_a = a.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2)
                throw std::runtime_error("Input must have shape (N, 2)");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sr = simd_f128_cos(sa);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise cos of (N, 2) arrays");

    m.def(
        "exp_arrays",
        [](py::array_t<double> a) -> py::array_t<double> {
            py::buffer_info buf_a = a.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2)
                throw std::runtime_error("Input must have shape (N, 2)");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sr = simd_f128_exp(sa);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise exp of (N, 2) arrays");

    m.def(
        "log_arrays",
        [](py::array_t<double> a) -> py::array_t<double> {
            py::buffer_info buf_a = a.request();
            if (buf_a.ndim != 2 || buf_a.shape[1] != 2)
                throw std::runtime_error("Input must have shape (N, 2)");

            size_t n = buf_a.shape[0];
            py::array_t<double> result({n, (size_t)2});
            py::buffer_info buf_res = result.request();

            double* ptr_a = static_cast<double*>(buf_a.ptr);
            double* ptr_res = static_cast<double*>(buf_res.ptr);

            for (size_t i = 0; i < n; i++) {
                simd_f128 sa = simd_f128_from_hi_lo(ptr_a[i * 2], ptr_a[i * 2 + 1]);
                simd_f128 sr = simd_f128_log(sa);
                simd_f128_extract(sr, &ptr_res[i * 2], &ptr_res[i * 2 + 1]);
            }
            return result;
        },
        "element-wise log of (N, 2) arrays");
}
