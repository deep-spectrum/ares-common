/**
 * @file test_from_kwargs.cpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/pyutil.hpp>
#include <test_from_kwargs.hpp>

KwArgsTestStruct::KwArgsTestStruct(const py::kwargs &kwargs) {
    ares::from_kwargs(kwargs,
                      // Basic types
                      SP(d), SP(str), SP(l), SP(i), SP(s), SP(c),

                      // Named types
                      SP_NAMED(i_initial, i0), SP_NAMED(bar_value, b.value),
                      SP_NAMED(foo_s, f.s), SP_NAMED(foobar_s, f.b.bar_s),
                      SP_NAMED(other, b.other),

                      // Containers
                      SP(other, f.b), SP(foo_l, f), SP(bar_s, b),
                      SP(value, f.b));
}

std::string KwArgsTestStruct::get_foobar_s() { return f.b.bar_s; }

int KwArgsTestStruct::get_foobar_value() const { return f.b.value; }

int KwArgsTestStruct::get_other() const { return f.b.other; }

std::string KwArgsTestStruct::get_foo_s() const { return f.s; }

long KwArgsTestStruct::get_foo_l() const { return f.foo_l; }

std::string KwArgsTestStruct::get_bar_s() const { return b.bar_s; }

int KwArgsTestStruct::get_bar_value() const { return b.value; }

int KwArgsTestStruct::get_bar_other() const { return b.other; }

double KwArgsTestStruct::get_d() const { return d; }

std::string KwArgsTestStruct::get_str() const { return str; }

long KwArgsTestStruct::get_l() const { return l; }

int KwArgsTestStruct::get_i() const { return i; }

int KwArgsTestStruct::get_initial_i() const { return i0; }

short KwArgsTestStruct::get_s() const { return s; }

char KwArgsTestStruct::get_c() const { return c; }
