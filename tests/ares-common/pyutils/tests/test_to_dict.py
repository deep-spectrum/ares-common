from pyutils_test_core import ToDictTest, TODICT_COMMON_DROP_VAL, TODICT_DROP_A_VAL, TODICT_DROP_B_VAL, \
    TODICT_DROP_C_VAL, TODICT_DROP_D_VAL, TODICT_DROP_E_VAL
import random
import pytest


def test_basic_dict0_random_values():
    expected = {
        "a": random.randint(-65535, 65535),
        "b": random.randint(-65535, 65535),
        "c": random.randint(-65535, 65535),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.basic_dict0()
    assert expected == result

def test_basic_dict0_common_drop_value():
    # Test that no common drop checks are taking place
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": TODICT_COMMON_DROP_VAL,
        "c": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.basic_dict0()
    assert expected == result


def test_basic_dict0_individual_drop_value():
    # Test that no drop checks are taking place
    expected = {
        "a": TODICT_DROP_A_VAL,
        "b": TODICT_DROP_B_VAL,
        "c": TODICT_DROP_C_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.basic_dict0()
    assert expected == result


def test_basic_dict1_random_values():
    expected = {
        "d": random.randint(-65535, 65535),
        "e": random.randint(-65535, 65535),
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.basic_dict1()
    assert expected == result

def test_basic_dict1_common_drop_value():
    # Test that no common drop checks are taking place
    expected = {
        "d": TODICT_COMMON_DROP_VAL,
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.basic_dict1()
    assert expected == result


def test_basic_dict1_individual_drop_value():
    # Test that no drop checks are taking place
    expected = {
        "d": TODICT_DROP_D_VAL,
        "e": TODICT_DROP_E_VAL,
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.basic_dict1()
    assert expected == result


def test_basic_dict2_random_values():
    expected = {
        "a": random.randint(-65535, 65535),
        "b": random.randint(-65535, 65535),
        "c": random.randint(-65535, 65535),
        "d": random.randint(-65535, 65535),
        "e": random.randint(-65535, 65535),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.basic_dict2()
    assert expected == result

def test_basic_dict2_common_drop_value():
    # Test that no common drop checks are taking place
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": TODICT_COMMON_DROP_VAL,
        "c": TODICT_COMMON_DROP_VAL,
        "d": TODICT_COMMON_DROP_VAL,
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.basic_dict2()
    assert expected == result


def test_basic_dict2_individual_drop_value():
    # Test that no drop checks are taking place
    expected = {
        "a": TODICT_DROP_A_VAL,
        "b": TODICT_DROP_B_VAL,
        "c": TODICT_DROP_C_VAL,
        "d": TODICT_DROP_D_VAL,
        "e": TODICT_DROP_E_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.basic_dict2()
    assert expected == result
