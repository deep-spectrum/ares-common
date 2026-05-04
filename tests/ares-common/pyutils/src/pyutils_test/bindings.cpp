/**
 * @file bindings.cpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <pybind11/pybind11.h>
#include <test_array_to_tuple.hpp>

namespace py = pybind11;

PYBIND11_MODULE(_pyutils_test, m, py::mod_gil_not_used()) {
    // array_to_tuple
    m.def("static_c_array_tuple", &test_static_C_style_array,
          py::arg("start_value"));
    m.def("dynamic_c_array_tuple", &test_dynamic_C_array,
          py::arg("start_value"), py::arg("count"));
    m.def("dynamic_cpp_array_tuple", &test_dynamic_CPP_array,
          py::arg("start_value"), py::arg("count"));
    m.def("cpp_array", &test_cpp_array, py::arg("start_value"));
    m.def("cpp_vector", &test_cpp_vector, py::arg("start_value"),
          py::arg("count"));

    m.attr("C_ARRAY_SIZE") = C_STYLE_ARRAY_SIZE;
    m.attr("CPP_ARRAY_SIZE") = CPP_STDLIB_ARRAY_SIZE;
}
