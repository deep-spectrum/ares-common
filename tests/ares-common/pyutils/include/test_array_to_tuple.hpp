/**
 * @file test_array_to_tuple.hpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_TEST_ARRAY_TO_TUPLE_HPP
#define ARES_TEST_ARRAY_TO_TUPLE_HPP

#include <pybind11/pybind11.h>

namespace py = pybind11;

#define C_STYLE_ARRAY_SIZE    10
#define CPP_STDLIB_ARRAY_SIZE 10

typedef int test_type_t;

py::tuple test_static_C_style_array(test_type_t start_value);
py::tuple test_dynamic_C_array(test_type_t start_value, int count);
py::tuple test_dynamic_CPP_array(test_type_t start_value, int count);
py::tuple test_cpp_array(test_type_t start_value);
py::tuple test_cpp_vector(test_type_t start_value, int count);
py::tuple test_single_value(test_type_t start_value);
py::tuple test_no_values();

#endif // ARES_TEST_ARRAY_TO_TUPLE_HPP
