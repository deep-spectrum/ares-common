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

py::tuple test_static_C_style_array(int start_value);
py::tuple test_dynamic_C_array(int start_value, int count);
py::tuple test_dynamic_CPP_array(int start_value, int count);
py::tuple test_cpp_array(int start_value);
py::tuple test_cpp_vector(int start_value, int count);

#endif // ARES_TEST_ARRAY_TO_TUPLE_HPP
