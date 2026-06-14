
/*
 * simd_f128_python.cpp -- high-performance 128-bit (double-double) arithmetic for python.
 * project url: https://github.com/tiw302/simd-f128
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 * python bindings:
 * ----------------
 * this file utilizes pybind11 to securely expose the underlying c++
 * floating point math into the python ecosystem, mapping operator overloads
 * and transcendental functions without precision loss.
 * license:
 * --------
 * mit license
 * copyright (c) 2026 jirawat siripuk
 * */

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.hpp"
#include "../include/simd_f128_complex.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simd_f128, m) {
    m.doc() = "simd-f128: 128-bit double-double arithmetic module";

    // ███████ ██       ██████   █████  ████████
    // ██      ██      ██    ██ ██   ██    ██
    // █████   ██      ██    ██ ███████    ██
    // ██      ██      ██    ██ ██   ██    ██
    // ██      ███████  ██████  ██   ██    ██
    //
    // >>class float128
    py::class_<f128::float128>(m, "Float128")
        .def(py::init<>())
        .def(py::init<double>())
        // bypass pybind11's default float cast (which truncates to 64-bit) by
        // catching string initializers via lambda and routing directly to the c-core parser.
        .def(py::init([](const std::string& s) { return f128::float128(simd_f128_from_string(s.c_str())); }))
        .def("to_string", &f128::float128::to_string)
        .def("__str__", &f128::float128::to_string)
        // override __repr__ to yield a valid python eval() expression that precisely
        // reconstructs the float128 state from a high-precision string literal.
        .def("__repr__", [](const f128::float128& self) { return "Float128('" + self.to_string() + "')"; })
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

    //  ██████  ██████  ███    ███ ██████  ██      ███████ ██   ██
    // ██      ██    ██ ████  ████ ██   ██ ██      ██       ██ ██
    // ██      ██    ██ ██ ████ ██ ██████  ██      █████     ███
    // ██      ██    ██ ██  ██  ██ ██      ██      ██       ██ ██
    //  ██████  ██████  ██      ██ ██      ███████ ███████ ██   ██
    //
    // >>class complex128
    py::class_<f128::complex128>(m, "Complex128")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("real"), py::arg("imag") = 0.0)
        .def(py::init<f128::float128, f128::float128>(), py::arg("real"), py::arg("imag") = f128::float128(0.0))
        .def("real", &f128::complex128::real)
        .def("imag", &f128::complex128::imag)
        .def("to_string", &f128::complex128::to_string)
        .def("__str__", &f128::complex128::to_string)
        .def("__repr__", &f128::complex128::to_string)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= py::self);

    // ███    ███  █████  ████████ ██   ██
    // ████  ████ ██   ██    ██    ██   ██
    // ██ ████ ██ ███████    ██    ███████
    // ██  ██  ██ ██   ██    ██    ██   ██
    // ██      ██ ██   ██    ██    ██   ██
    //
    // >>math functions
    //
    // expose transcendental, geometric, and logical functions into the python
    // module namespace. overloads are resolved via static_cast for functions
    // that operate on both float128 and complex128 domains.
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

    // ████████ ██    ██ ██████  ███████
    //    ██     ██  ██  ██   ██ ██
    //    ██      ████   ██████  █████
    //    ██       ██    ██      ██
    //    ██       ██    ██      ███████
    //
    // >>implicit conversions
    //
    // register seamless automatic type promotion from standard python complex
    // (which is backed by c++ std::complex<double>) up to our custom complex128.
    py::implicitly_convertible<std::complex<double>, f128::complex128>();

    //  ██████  ██████  ███    ██ ███████ ████████
    // ██      ██    ██ ████   ██ ██         ██
    // ██      ██    ██ ██ ██  ██ ███████    ██
    // ██      ██    ██ ██  ██ ██      ██    ██
    //  ██████  ██████  ██   ████ ███████    ██
    //
    // >>constants
    //
    // pre-computed mathematically exact constants initialized directly into
    // the python module attributes.
    m.attr("pi") = f128::pi;
    m.attr("e") = f128::e;
    m.attr("sqrt2") = f128::sqrt2;
    m.attr("ln2") = f128::ln2;
}
