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


def test_basic_dict3_random_values():
    expected = {
        "a": random.randint(-65535, 65535),
        "b": random.randint(-65535, 65535),
        "c": random.randint(-65535, 65535),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.basic_dict3()
    assert expected == result

def test_basic_dict3_common_drop_value():
    # Test that no common drop checks are taking place
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": TODICT_COMMON_DROP_VAL,
        "c": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.basic_dict3()
    assert expected == result


def test_basic_dict3_individual_drop_value():
    # Test that no drop checks are taking place
    expected = {
        "a": TODICT_DROP_A_VAL,
        "b": TODICT_DROP_B_VAL,
        "c": TODICT_DROP_C_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.basic_dict3()
    assert expected == result


def test_basic_dict4_random_values():
    expected = {
        "d": random.randint(-65535, 65535),
        "e": random.randint(-65535, 65535),
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.basic_dict4()
    assert expected == result

def test_basic_dict4_common_drop_value():
    # Test that no common drop checks are taking place
    expected = {
        "d": TODICT_COMMON_DROP_VAL,
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.basic_dict4()
    assert expected == result


def test_basic_dict4_individual_drop_value():
    # Test that no drop checks are taking place
    expected = {
        "d": TODICT_DROP_D_VAL,
        "e": TODICT_DROP_E_VAL,
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.basic_dict4()
    assert expected == result


def test_basic_dict5_random_values():
    expected = {
        "a": random.randint(-65535, 65535),
        "b": random.randint(-65535, 65535),
        "c": random.randint(-65535, 65535),
        "d": random.randint(-65535, 65535),
        "e": random.randint(-65535, 65535),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.basic_dict5()
    assert expected == result

def test_basic_dict5_common_drop_value():
    # Test that no common drop checks are taking place
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": TODICT_COMMON_DROP_VAL,
        "c": TODICT_COMMON_DROP_VAL,
        "d": TODICT_COMMON_DROP_VAL,
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.basic_dict5()
    assert expected == result


def test_basic_dict5_individual_drop_value():
    # Test that no drop checks are taking place
    expected = {
        "a": TODICT_DROP_A_VAL,
        "b": TODICT_DROP_B_VAL,
        "c": TODICT_DROP_C_VAL,
        "d": TODICT_DROP_D_VAL,
        "e": TODICT_DROP_E_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.basic_dict5()
    assert expected == result


def test_conditional_dict0_non_drop_values():
    sample_pool = [int(x) for x in range(-65535, 65535)]
    sample_pool.remove(TODICT_COMMON_DROP_VAL)
    expected = {
        "a": random.choice(sample_pool),
        "b": random.choice(sample_pool),
        "c": random.choice(sample_pool),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    result = cut.conditional_dict0()
    assert expected == result


def test_conditional_dict0_drop_single_values():
    # Drop a single value. This applies to common drop only
    sample_pool = [int(x) for x in range(-65535, 65535)]
    sample_pool.remove(TODICT_COMMON_DROP_VAL)
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": random.choice(sample_pool),
        "c": random.choice(sample_pool),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    expected["a"] = None
    result = cut.conditional_dict0()
    assert expected == result

    expected = {
        "a": random.choice(sample_pool),
        "b": TODICT_COMMON_DROP_VAL,
        "c": random.choice(sample_pool),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    expected["b"] = None
    result = cut.conditional_dict0()
    assert expected == result

    expected = {
        "a": random.choice(sample_pool),
        "b": random.choice(sample_pool),
        "c": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    expected["c"] = None
    result = cut.conditional_dict0()
    assert expected == result


def test_conditional_dict0_drop_all_values():
    # Drop all values. This applies to common drop only
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": TODICT_COMMON_DROP_VAL,
        "c": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], 0, 0)
    expected["a"] = None
    expected["b"] = None
    expected["c"] = None
    result = cut.conditional_dict0()
    assert expected == result


def test_conditional_dict1_non_drop_values():
    sample_pool = [int(x) for x in range(-65535, 65535)]
    sample_pool.remove(TODICT_COMMON_DROP_VAL)
    expected = {
        "d": random.choice(sample_pool),
        "e": random.choice(sample_pool),
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    result = cut.conditional_dict1()
    assert expected == result


def test_conditional_dict1_drop_single_values():
    # Drop a single value. This applies to common drop only
    sample_pool = [int(x) for x in range(-65535, 65535)]
    sample_pool.remove(TODICT_COMMON_DROP_VAL)
    expected = {
        "d": TODICT_COMMON_DROP_VAL,
        "e": random.choice(sample_pool),
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    expected["d"] = None
    result = cut.conditional_dict1()
    assert expected == result

    expected = {
        "d": random.choice(sample_pool),
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    expected["e"] = None
    result = cut.conditional_dict1()
    assert expected == result


def test_conditional_dict1_drop_all_values():
    # Drop all values. This applies to common drop only
    expected = {
        "d": TODICT_COMMON_DROP_VAL,
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(0, 0, 0, expected["d"], expected["e"])
    expected["d"] = None
    expected["e"] = None
    result = cut.conditional_dict1()
    assert expected == result


def test_conditional_dict2_non_drop_values():
    sample_pool = [int(x) for x in range(-65535, 65535)]
    sample_pool.remove(TODICT_COMMON_DROP_VAL)
    expected = {
        "a": random.choice(sample_pool),
        "b": random.choice(sample_pool),
        "c": random.choice(sample_pool),
        "d": random.choice(sample_pool),
        "e": random.choice(sample_pool),
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    result = cut.conditional_dict2()
    assert expected == result


def test_conditional_dict2_drop_single_values():
    # Drop a single value. This applies to common drop only
    sample_pool = [int(x) for x in range(-65535, 65535)]
    sample_pool.remove(TODICT_COMMON_DROP_VAL)
    expected = {
        "a": random.choice(sample_pool),
        "b": random.choice(sample_pool),
        "c": random.choice(sample_pool),
        "d": random.choice(sample_pool),
        "e": random.choice(sample_pool),
    }

    for key in expected.keys():
        expected[key] = TODICT_COMMON_DROP_VAL
        cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
        expected[key] = None
        result = cut.conditional_dict2()
        assert expected == result
        expected[key] = random.choice(sample_pool)


def test_conditional_dict2_drop_all_values():
    # Drop all values. This applies to common drop only
    expected = {
        "a": TODICT_COMMON_DROP_VAL,
        "b": TODICT_COMMON_DROP_VAL,
        "c": TODICT_COMMON_DROP_VAL,
        "d": TODICT_COMMON_DROP_VAL,
        "e": TODICT_COMMON_DROP_VAL,
    }
    cut = ToDictTest(expected["a"], expected["b"], expected["c"], expected["d"], expected["e"])
    expected["a"] = None
    expected["b"] = None
    expected["c"] = None
    expected["d"] = None
    expected["e"] = None
    result = cut.conditional_dict2()
    assert expected == result
