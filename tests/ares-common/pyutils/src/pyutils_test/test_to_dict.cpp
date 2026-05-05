/**
 * @file test_to_dict.cpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <test_to_dict.hpp>

#include "ares/pyutil.hpp"

py::dict ToDictTest::basic_dict0() {
    return ares::to_dict(NV_NO_CHECK(a), NV_NO_CHECK(b), NV_NO_CHECK(c));
}

py::dict ToDictTest::basic_dict1() { return ares::to_dict(NV_NO_CHECK(d, fb)); }

py::dict ToDictTest::basic_dict2() {
    return ares::to_dict(NV_NO_CHECK(a), NV_NO_CHECK(b), NV_NO_CHECK(c),
                         NV_NO_CHECK(d, fb));
}

py::dict ToDictTest::basic_dict3() {
    return ares::to_dict(NV(a), NV(b), NV(c));
}

py::dict ToDictTest::basic_dict4() { return ares::to_dict(NV(d, fb)); }

py::dict ToDictTest::basic_dict5() {
    return ares::to_dict(NV(a), NV(b), NV(c), NV(d, fb));
}
