from pyutils_test_core import static_c_array_tuple, dynamic_c_array_tuple, dynamic_cpp_array_tuple, cpp_array, \
    cpp_vector, C_ARRAY_SIZE, CPP_ARRAY_SIZE, tuple_single_value, tuple_no_value
import random
import pytest


@pytest.mark.parametrize('execution_number', range(10))
def test_static_c_array(execution_number):
    start = random.randint(-100, 100)
    expected = tuple([int(x) for x in range(start, start + C_ARRAY_SIZE, 1)])
    result = static_c_array_tuple(start)
    assert expected == result


@pytest.mark.parametrize('execution_number', range(10))
def test_dynamic_c_array(execution_number):
    start = random.randint(-100, 100)
    size = random.randint(2, 10000)
    expected = tuple([int(x) for x in range(start, start + size, 1)])
    result = dynamic_c_array_tuple(start, size)
    assert expected == result


@pytest.mark.parametrize('execution_number', range(10))
def test_dynamic_cpp_array(execution_number):
    start = random.randint(-100, 100)
    size = random.randint(2, 10000)
    expected = tuple([int(x) for x in range(start, start + size, 1)])
    result = dynamic_cpp_array_tuple(start, size)
    assert expected == result


@pytest.mark.parametrize('execution_number', range(10))
def test_cpp_array(execution_number):
    start = random.randint(-100, 100)
    expected = tuple([int(x) for x in range(start, start + CPP_ARRAY_SIZE, 1)])
    result = cpp_array(start)
    assert expected == result


@pytest.mark.parametrize('execution_number', range(10))
def test_cpp_vector(execution_number):
    start = random.randint(-100, 100)
    size = random.randint(2, 10000)
    expected = tuple([int(x) for x in range(start, start + size, 1)])
    result = cpp_vector(start, size)
    assert expected == result

def test_single_value():
    start = random.randint(-100, 100)
    val = tuple_single_value(start)
    assert tuple([start]) == val


def test_empty():
    val = tuple_no_value()
    assert not val
