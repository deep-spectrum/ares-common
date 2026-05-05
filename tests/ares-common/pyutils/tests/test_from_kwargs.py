from pyutils_test_core import KwArgsTest, KWARGS_DEFAULT_BAR_S, KWARGS_DEFAULT_BAR_OTHER, KWARGS_DEFAULT_BAR_VALUE, \
    KWARGS_DEFAULT_C, KWARGS_DEFAULT_D, KWARGS_DEFAULT_FOO_L, KWARGS_DEFAULT_FOO_S, KWARGS_DEFAULT_I, \
    KWARGS_DEFAULT_INITIAL_I, KWARGS_DEFAULT_L, KWARGS_DEFAULT_S, KWARGS_DEFAULT_STR
import random
import string
import pytest


def gen_random_str(max_len: int = 512):
    str_len = random.randint(1, max_len)
    return ''.join(random.choices(string.printable, k=str_len))


def test_no_kwargs():
    s = KwArgsTest()
    assert s.foobar_s == KWARGS_DEFAULT_BAR_S
    assert s.foobar_value == KWARGS_DEFAULT_BAR_VALUE
    assert s.other == KWARGS_DEFAULT_BAR_OTHER
    assert s.foo_s == KWARGS_DEFAULT_FOO_S
    assert s.foo_l == KWARGS_DEFAULT_FOO_L
    assert s.bar_s == KWARGS_DEFAULT_BAR_S
    assert s.bar_value == KWARGS_DEFAULT_BAR_VALUE
    assert s.bar_other == KWARGS_DEFAULT_BAR_OTHER
    assert s.d == KWARGS_DEFAULT_D
    assert s.str == KWARGS_DEFAULT_STR
    assert s.l == KWARGS_DEFAULT_L
    assert s.i == KWARGS_DEFAULT_I
    assert s.initial_i == KWARGS_DEFAULT_INITIAL_I
    assert s.s == KWARGS_DEFAULT_S
    assert s.c == KWARGS_DEFAULT_C


def test_kwargs_basic():
    d = random.uniform(-100.0, 100.0)
    str_ = gen_random_str()
    l = random.randint(-65535, 65535)
    i = random.randint(-1024, 1024)
    s = random.randint(-512, 512)
    c = random.randint(-63, 63)

    test = KwArgsTest(
        d=d,
        str=str_,
        l=l,
        i=i,
        s=s,
        c=c,
    )

    assert test.d == pytest.approx(d)
    assert test.str == str_
    assert test.l == l
    assert test.i == i
    assert test.s == s
    assert test.c == c

    assert test.foobar_s == KWARGS_DEFAULT_BAR_S
    assert test.foobar_value == KWARGS_DEFAULT_BAR_VALUE
    assert test.other == KWARGS_DEFAULT_BAR_OTHER
    assert test.foo_s == KWARGS_DEFAULT_FOO_S
    assert test.foo_l == KWARGS_DEFAULT_FOO_L
    assert test.bar_s == KWARGS_DEFAULT_BAR_S
    assert test.bar_value == KWARGS_DEFAULT_BAR_VALUE
    assert test.bar_other == KWARGS_DEFAULT_BAR_OTHER
    assert test.initial_i == KWARGS_DEFAULT_INITIAL_I


def test_kwargs_named():
    initial_i = random.randint(-65535, 65535)
    bar_value = random.randint(-65535, 65535)
    foo_s = gen_random_str()
    foobar_s = gen_random_str()
    bar_other = random.randint(-65535, 65535)

    test = KwArgsTest(
        initial_i=initial_i,
        bar_value=bar_value,
        foo_s=foo_s,
        foobar_s=foobar_s,
        bar_other=bar_other,
    )

    assert test.initial_i == initial_i
    assert test.bar_value == bar_value
    assert test.foo_s == foo_s
    assert test.foobar_s == foobar_s
    assert test.bar_other == bar_other

    assert test.foobar_value == KWARGS_DEFAULT_BAR_VALUE
    assert test.foo_l == KWARGS_DEFAULT_FOO_L
    assert test.bar_s == KWARGS_DEFAULT_BAR_S
    assert test.other == KWARGS_DEFAULT_BAR_OTHER
    assert test.d == KWARGS_DEFAULT_D
    assert test.str == KWARGS_DEFAULT_STR
    assert test.l == KWARGS_DEFAULT_L
    assert test.i == KWARGS_DEFAULT_I
    assert test.s == KWARGS_DEFAULT_S
    assert test.c == KWARGS_DEFAULT_C


def test_kwargs_containers():
    other = random.randint(-65535, 65535)
    foo_l = random.randint(-65535, 65535)
    bar_s = gen_random_str()
    value = random.randint(-65535, 65535)

    test = KwArgsTest(
        other=other,
        foo_l=foo_l,
        bar_s=bar_s,
        value=value,
    )

    assert test.other == other
    assert test.foo_l == foo_l
    assert test.bar_s == bar_s
    assert test.foobar_value == value

    assert test.foobar_s == KWARGS_DEFAULT_BAR_S
    assert test.foo_s == KWARGS_DEFAULT_FOO_S
    assert test.bar_value == KWARGS_DEFAULT_BAR_VALUE
    assert test.bar_other == KWARGS_DEFAULT_BAR_OTHER
    assert test.d == KWARGS_DEFAULT_D
    assert test.str == KWARGS_DEFAULT_STR
    assert test.l == KWARGS_DEFAULT_L
    assert test.i == KWARGS_DEFAULT_I
    assert test.initial_i == KWARGS_DEFAULT_INITIAL_I
    assert test.s == KWARGS_DEFAULT_S
    assert test.c == KWARGS_DEFAULT_C
