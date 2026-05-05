/**
 * @file test_to_dict.hpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_TEST_TO_DICT_HPP
#define ARES_TEST_TO_DICT_HPP

#include <pybind11/pybind11.h>

namespace py = pybind11;

struct FooBar {
    FooBar() = default;
    explicit FooBar(int i) : d(i) {}
    int d = 4;
};

struct ToDictTest {
    ToDictTest() = default;
    explicit ToDictTest(int i0, int i1, int i2, int i3)
        : a(i0), b(i1), c(i2), fb(i3) {}

    int a = 0;
    int b = 1;
    int c = 2;
    FooBar fb;

    // named value no check
    py::dict basic_dict0(); // field and container the same
    py::dict basic_dict1(); // field and container different
    py::dict basic_dict2(); // combine 0 and 1

    // named value
    py::dict basic_dict3(); // field and container the same
    py::dict basic_dict4(); // field and container different
    py::dict basic_dict5(); // combine 0 and 1

    py::dict conditional_dict0();
};

#endif // ARES_TEST_TO_DICT_HPP
