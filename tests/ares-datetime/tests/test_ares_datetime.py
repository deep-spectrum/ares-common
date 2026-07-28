from datetime_test_core import py_datetime_to_ares_datetime, py_datetime_to_ares_datetime_and_back
from datetime import datetime, timezone


def test_ares_datetime():
    now = datetime.now()
    ares_now = py_datetime_to_ares_datetime(now)
    assert now.year == ares_now[0]
    assert now.month == ares_now[1]
    assert now.day == ares_now[2]
    assert now.hour == ares_now[3]
    assert now.minute == ares_now[4]
    assert now.second == ares_now[5]

def test_ares_datetime_time_point():
    now = datetime.now()
    ares_dt = py_datetime_to_ares_datetime_and_back(now)
    assert now == ares_dt
