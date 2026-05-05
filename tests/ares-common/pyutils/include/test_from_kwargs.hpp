/**
 * @file test_from_kwargs.hpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_TEST_FROM_KWARGS_HPP
#define ARES_TEST_FROM_KWARGS_HPP

#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

constexpr const char *default_bar_s = "Bar";
constexpr int default_bar_value = 0;
constexpr int default_bar_other = 1;

constexpr const char *default_foo_s = "Foo";
constexpr long default_foo_l = -100;

constexpr double default_d = 0.0;
constexpr const char *default_str = "";
constexpr long default_l = 700;
constexpr int default_i = 900;
constexpr int default_initial_i = 0;
constexpr short default_s = 5;
constexpr int8_t default_c = -127;

struct Bar {
    std::string bar_s = default_bar_s;
    int value = default_bar_value;
    int other = default_bar_other;
};

struct Foo {
    Bar b;
    std::string s = default_foo_s;
    long foo_l = default_foo_l;
};

struct KwArgsTestStruct {
    Foo f;
    Bar b;
    double d = default_d;
    std::string str = default_str;
    long l = default_l;
    int i = default_i;
    int i0 = default_initial_i;
    short s = default_s;
    int8_t c = default_c;

    explicit KwArgsTestStruct(const py::kwargs &kwargs);

    std::string get_foobar_s();   /// f.b.bar_s (foobar_s [named])
    int get_foobar_value() const; /// f.b.value (value [container])
    int get_other() const;        /// f.b.other (other [container])

    std::string get_foo_s() const; /// f.foo_s (foo_s [named])
    long get_foo_l() const;        /// f.foo_l (foo_l [container])

    std::string get_bar_s() const; /// b.bar_s (bar_s [container])
    int get_bar_value() const;     /// b.value (bar_value [named])
    int get_bar_other() const;     /// b.other (other [named])

    double get_d() const;        /// d
    std::string get_str() const; /// str
    long get_l() const;          /// l
    int get_i() const;           /// i
    int get_initial_i() const;   /// i0 (i_initial [named])
    short get_s() const;         /// s
    int8_t get_c() const;          /// c
};

#endif // ARES_TEST_FROM_KWARGS_HPP
