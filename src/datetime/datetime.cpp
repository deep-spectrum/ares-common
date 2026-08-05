/**
 * @file datetime.cpp
 *
 * @brief
 *
 * @date 7/28/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/datetime/datetime.hpp>
#include <mutex>

namespace ares {
static std::tm *localtime_safe(const std::time_t *time, std::tm *buf) {
#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) ||         \
    defined(_MSC_VER)
    if (localtime_s(buf, time)) {
        return nullptr;
    }
    return buf;
#else
    static std::mutex mtx;
    std::lock_guard lock(mtx);
    std::tm *tm_ptr = std::localtime(time);
    if (tm_ptr != nullptr) {
        *buf = *tm_ptr;
    }
    return tm_ptr;
#endif
}

DateTime::DateTime(const DateTime &rhs) {
    _year = rhs._year;
    _month = rhs._month;
    _day = rhs._day;
    _hour = rhs._hour;
    _minute = rhs._minute;
    _second = rhs._second;
    _microsecond = rhs._microsecond;
    _datetime_to_timepoint();
}

DateTime::DateTime(const std::chrono::system_clock::time_point &timepoint) {
    _timepoint = timepoint;
    _timepoint_to_datetime(timepoint);
}

DateTime::DateTime(int year, int month, int day, int hour, int minute,
                   int second, int microsecond) {
    _year = year;
    _month = month;
    _day = day;
    _hour = hour;
    _minute = minute;
    _second = second;
    _microsecond = microsecond;
    _datetime_to_timepoint();
}

int DateTime::year() const { return _year; }

int DateTime::month() const { return _month; }

int DateTime::day() const { return _day; }

int DateTime::hour() const { return _hour; }

int DateTime::minute() const { return _minute; }

int DateTime::second() const { return _second; }

int DateTime::microsecond() const { return _microsecond; }

std::chrono::system_clock::time_point DateTime::time_point() const {
    return _timepoint;
}

DateTime &DateTime::operator=(const DateTime &rhs) {
    _year = rhs._year;
    _month = rhs._month;
    _day = rhs._day;
    _hour = rhs._hour;
    _minute = rhs._minute;
    _second = rhs._second;
    _microsecond = rhs._microsecond;
    _datetime_to_timepoint();
    return *this;
}

void DateTime::_timepoint_to_datetime(
    const std::chrono::system_clock::time_point &timepoint) {
    using namespace std::chrono;
    using us_t = duration<int, std::micro>;
    auto us = duration_cast<us_t>(timepoint.time_since_epoch() % seconds(1));
    if (us.count() < 0) {
        us += duration_cast<us_t>(seconds(1));
    }

    const std::time_t tt = system_clock::to_time_t(
        time_point_cast<system_clock::duration>(timepoint - us));

    std::tm localtime;
    std::tm *localtime_ptr = localtime_safe(&tt, &localtime);
    if (localtime_ptr == nullptr) {
        throw DateTimeException(
            "Unable to represent system_lock in local time");
    }

    _year = localtime.tm_year + 1900;
    _month = localtime.tm_mon + 1;
    _day = localtime.tm_mday;
    _hour = localtime.tm_hour;
    _minute = localtime.tm_min;
    _second = localtime.tm_sec;
    _microsecond = us.count();
}

void DateTime::_datetime_to_timepoint() {
    using namespace std::chrono;

    std::tm cal{
        .tm_sec = _second,
        .tm_min = _minute,
        .tm_hour = _hour,
        .tm_mday = _day,
        .tm_mon = _month - 1,
        .tm_year = _year - 1900,
        .tm_isdst = -1,
    };
    microseconds msecs(_microsecond);

    _timepoint = system_clock::from_time_t(std::mktime(&cal)) + msecs;
}
} // namespace ares
