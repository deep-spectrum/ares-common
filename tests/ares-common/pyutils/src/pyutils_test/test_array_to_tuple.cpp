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
#include <test_array_to_tuple.hpp>
#include <vector>

template <typename T>
static void init_array(T &arr, size_t count, test_type_t start_value) {
    int value = start_value;

    for (size_t i = 0u; i < count; i++, value++) {
        arr[i] = value;
    }
}

py::tuple test_static_C_style_array(test_type_t start_value) {
    int arr[C_STYLE_ARRAY_SIZE];
    init_array(arr, C_STYLE_ARRAY_SIZE, start_value);
    return ares::array_to_tuple(arr, C_STYLE_ARRAY_SIZE);
}

py::tuple test_dynamic_C_array(test_type_t start_value, int count) {
    auto arr = static_cast<test_type_t *>(calloc(count, sizeof(test_type_t)));

    if (arr == nullptr) {
        throw std::bad_alloc();
    }

    init_array(arr, count, start_value);
    py::tuple ret = ares::array_to_tuple(arr, count);
    memset(arr, 0, sizeof(test_type_t) * count);
    free(arr);

    return ret;
}

py::tuple test_dynamic_CPP_array(test_type_t start_value, int count) {
    auto arr = new test_type_t[count];

    init_array(arr, count, start_value);
    py::tuple ret = ares::array_to_tuple(arr, count);
    memset(arr, 0, sizeof(test_type_t) * count);
    delete[] arr;

    return ret;
}

py::tuple test_cpp_array(test_type_t start_value) {
    std::array<test_type_t, CPP_STDLIB_ARRAY_SIZE> arr;
    init_array(arr, CPP_STDLIB_ARRAY_SIZE, start_value);
    return ares::array_to_tuple(arr.data(), arr.size());
}

py::tuple test_cpp_vector(test_type_t start_value, int count) {
    std::vector<test_type_t> arr(count);
    init_array(arr, count, start_value);
    return ares::array_to_tuple(arr.data(), arr.size());
}

py::tuple test_single_value(test_type_t start_value) {
    test_type_t value = start_value;
    return ares::array_to_tuple(&value, 1);
}

py::tuple test_no_values() {
    test_type_t *val = nullptr;
    return ares::array_to_tuple(val, 0);
}
