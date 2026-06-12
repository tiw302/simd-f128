#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.hpp"
#include "../include/simd_f128_complex.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simd_f128, m) {
    m.doc() = "simd-f128: 128-bit double-double arithmetic module";

    py::class_<f128::float128>(m, "Float128")
        .def(py::init<>())
        .def(py::init<double>())
        // use lambda to parse string natively to avoid python's 64-bit float truncation
        .def(py::init([](const std::string& s) { return f128::float128(simd_f128_from_string(s.c_str())); }))
        .def("to_string", &f128::float128::to_string)
        .def("__str__", &f128::float128::to_string)
        // repr returns valid code
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

    // math functions
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

    // implicit conversions
    py::implicitly_convertible<std::complex<double>, f128::complex128>();

    // constants
    m.attr("pi") = f128::pi;
    m.attr("e") = f128::e;
    m.attr("sqrt2") = f128::sqrt2;
    m.attr("ln2") = f128::ln2;
}
