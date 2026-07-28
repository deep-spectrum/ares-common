/**
 * @file bindings.cpp
 *
 * @brief
 *
 * @date 7/28/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/datetime/datetime.hpp>
#include <chrono>
#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

static py::tuple
py_datetime_to_ares_datetime(const std::chrono::system_clock::time_point &tp) {
    const ares::DateTime dt(tp);
    return py::make_tuple(dt.year(), dt.month(), dt.day(), dt.hour(),
                          dt.minute(), dt.second());
}

PYBIND11_MODULE(_datetime_test, m, py::mod_gil_not_used()) {
    m.def("py_datetime_to_ares_datetime", py_datetime_to_ares_datetime);
}