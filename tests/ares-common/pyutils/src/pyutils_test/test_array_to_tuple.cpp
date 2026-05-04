/**
 * @file test_array_to_tuple.cpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/pyutil.hpp>
#include <array>
#include <cstdlib>
#include <exception>
#include <pybind11/pybind11.h>
#include <vector>

namespace py = pybind11;

#define C_STYLE_ARRAY_SIZE 10

template <typename T>
static void init_array(T *arr, size_t count, int start_value) {
    int value = start_value;

    for (size_t i = 0u; i < count; i++, value++) {
        arr[i] = value;
    }
}

py::tuple test_static_C_style_array(int start_value) {
    uint64_t arr[C_STYLE_ARRAY_SIZE];
    init_array(arr, C_STYLE_ARRAY_SIZE, start_value);
    return ares::array_to_tuple(arr, C_STYLE_ARRAY_SIZE);
}

py::tuple test_dynamic_C_array(int start_value, int count) {
    auto arr = static_cast<int *>(calloc(count, sizeof(int)));

    if (arr == nullptr) {
        throw std::bad_alloc();
    }

    init_array(arr, count, start_value);
    py::tuple ret = ares::array_to_tuple(arr, count);
    memset(arr, 0, sizeof(int) * count);
    free(arr);

    return ret;
}

py::tuple test_dynamic_CPP_array(int start_value, int count) {
    auto arr = new int[count];

    init_array(arr, count, start_value);
    py::tuple ret = ares::array_to_tuple(arr, count);
    memset(arr, 0, sizeof(int) * count);
    delete[] arr;

    return ret;
}
