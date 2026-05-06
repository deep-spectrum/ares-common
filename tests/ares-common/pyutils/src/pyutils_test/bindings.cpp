/**
 * @file bindings.cpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#define PYBIND11_DETAILED_ERROR_MESSAGES

#include <pybind11/pybind11.h>
#include <test_array_to_tuple.hpp>
#include <test_from_kwargs.hpp>
#include <test_to_dict.hpp>

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
    m.def("tuple_single_value", &test_single_value, py::arg("start_value"));
    m.def("tuple_no_value", &test_no_values);

    m.attr("C_ARRAY_SIZE") = C_STYLE_ARRAY_SIZE;
    m.attr("CPP_ARRAY_SIZE") = CPP_STDLIB_ARRAY_SIZE;

    // from_kwargs
    py::class_<KwArgsTestStruct>(m, "KwArgsTest")
        .def(py::init<const py::kwargs &>())
        .def_property_readonly("foobar_s", &KwArgsTestStruct::get_foobar_s)
        .def_property_readonly("foobar_value",
                               &KwArgsTestStruct::get_foobar_value)
        .def_property_readonly("other", &KwArgsTestStruct::get_other)
        .def_property_readonly("foo_s", &KwArgsTestStruct::get_foo_s)
        .def_property_readonly("foo_l", &KwArgsTestStruct::get_foo_l)
        .def_property_readonly("bar_s", &KwArgsTestStruct::get_bar_s)
        .def_property_readonly("bar_value", &KwArgsTestStruct::get_bar_value)
        .def_property_readonly("bar_other", &KwArgsTestStruct::get_bar_other)
        .def_property_readonly("d", &KwArgsTestStruct::get_d)
        .def_property_readonly("str", &KwArgsTestStruct::get_str)
        .def_property_readonly("l", &KwArgsTestStruct::get_l)
        .def_property_readonly("i", &KwArgsTestStruct::get_i)
        .def_property_readonly("initial_i", &KwArgsTestStruct::get_initial_i)
        .def_property_readonly("s", &KwArgsTestStruct::get_s)
        .def_property_readonly("c", &KwArgsTestStruct::get_c);

    m.attr("KWARGS_DEFAULT_BAR_S") = default_bar_s;
    m.attr("KWARGS_DEFAULT_BAR_VALUE") = default_bar_value;
    m.attr("KWARGS_DEFAULT_BAR_OTHER") = default_bar_other;

    m.attr("KWARGS_DEFAULT_FOO_S") = default_foo_s;
    m.attr("KWARGS_DEFAULT_FOO_L") = default_foo_l;

    m.attr("KWARGS_DEFAULT_D") = default_d;
    m.attr("KWARGS_DEFAULT_STR") = default_str;
    m.attr("KWARGS_DEFAULT_L") = default_l;
    m.attr("KWARGS_DEFAULT_I") = default_i;
    m.attr("KWARGS_DEFAULT_INITIAL_I") = default_initial_i;
    m.attr("KWARGS_DEFAULT_S") = default_s;
    m.attr("KWARGS_DEFAULT_C") = default_c;

    // to_dict
    py::class_<ToDictTest>(m, "ToDictTest")
        .def(py::init<>())
        .def(py::init<int, int, int, int, int>(), py::arg("a"), py::arg("b"),
             py::arg("c"), py::arg("d"), py::arg("e"))
        .def("basic_dict0", &ToDictTest::basic_dict0)
        .def("basic_dict1", &ToDictTest::basic_dict1)
        .def("basic_dict2", &ToDictTest::basic_dict2)
        .def("basic_dict3", &ToDictTest::basic_dict3)
        .def("basic_dict4", &ToDictTest::basic_dict4)
        .def("basic_dict5", &ToDictTest::basic_dict5)
        .def("conditional_dict0", &ToDictTest::conditional_dict0)
        .def("conditional_dict1", &ToDictTest::conditional_dict1)
        .def("conditional_dict2", &ToDictTest::conditional_dict2)
        .def("conditional_dict3", &ToDictTest::conditional_dict3)
        .def("conditional_dict4", &ToDictTest::conditional_dict4)
        .def("conditional_dict5", &ToDictTest::conditional_dict5);

    m.attr("TODICT_COMMON_DROP_VAL") = common_drop_value;
    m.attr("TODICT_DROP_A_VAL") = drop_a_value;
    m.attr("TODICT_DROP_B_VAL") = drop_b_value;
    m.attr("TODICT_DROP_C_VAL") = drop_c_value;
    m.attr("TODICT_DROP_D_VAL") = drop_d_value;
    m.attr("TODICT_DROP_E_VAL") = drop_e_value;
}
