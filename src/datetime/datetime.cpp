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

namespace ares {
DateTime::DateTime(const std::chrono::system_clock::time_point &timepoint) {
    _timepoint_to_datetime(timepoint);
}

int DateTime::year() const { return _year; }

unsigned DateTime::month() const { return _month; }

unsigned DateTime::day() const { return _day; }

unsigned DateTime::hour() const { return _hour; }

unsigned DateTime::minute() const { return _minute; }

unsigned DateTime::second() const { return _second; }

void DateTime::_timepoint_to_datetime(
    const std::chrono::system_clock::time_point &timepoint) {
    const std::chrono::year_month_day ymd{
        std::chrono::floor<std::chrono::days>(timepoint)};
    const std::chrono::hh_mm_ss hms{
        timepoint - std::chrono::floor<std::chrono::days>(timepoint)};

    if (!ymd.ok()) {
        throw DateTimeException("Invalid date");
    }

    _year = static_cast<int>(ymd.year());
    _month = static_cast<unsigned>(ymd.month());
    _day = static_cast<unsigned>(ymd.day());
    _hour = hms.hours().count();
    _minute = hms.minutes().count();
    _second = hms.seconds().count();
}
} // namespace ares
