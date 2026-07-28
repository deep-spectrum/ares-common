/**
 * @file datetime.hpp
 *
 * @brief
 *
 * @date 7/28/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_DATETIME_HPP
#define ARES_DATETIME_HPP

#include <chrono>
#include <exception>
#include <string>

namespace ares {

class DateTimeException : std::exception {
  public:
    explicit DateTimeException(const char *msg) : _msg(msg) {}

    [[nodiscard]] const char *what() const noexcept override {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};

class DateTime {
  public:
    explicit DateTime(const std::chrono::system_clock::time_point &timepoint);
    explicit DateTime(int year, int month, int day, int hour, int minute,
                      int second, int microsecond = 0);

    [[nodiscard]] int year() const;
    [[nodiscard]] int month() const;
    [[nodiscard]] int day() const;
    [[nodiscard]] int hour() const;
    [[nodiscard]] int minute() const;
    [[nodiscard]] int second() const;
    [[nodiscard]] int microsecond() const;

    [[nodiscard]] std::chrono::system_clock::time_point time_point() const;

  private:
    std::chrono::system_clock::time_point _timepoint;
    int _year = 0;
    int _month = 0;
    int _day = 0;
    int _hour = 0;
    int _minute = 0;
    int _second = 0;
    int _microsecond = 0;

    void _timepoint_to_datetime(
        const std::chrono::system_clock::time_point &timepoint);

    void _datetime_to_timepoint();
};
} // namespace ares

#endif // ARES_DATETIME_HPP
