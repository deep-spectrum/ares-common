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

    [[nodiscard]] int year() const;
    [[nodiscard]] unsigned month() const;
    [[nodiscard]] unsigned day() const;
    [[nodiscard]] unsigned hour() const;
    [[nodiscard]] unsigned minute() const;
    [[nodiscard]] unsigned second() const;

  private:
    int _year = 0;
    unsigned _month = 0;
    unsigned _day = 0;
    unsigned _hour = 0;
    unsigned _minute = 0;
    unsigned _second = 0;

    void _timepoint_to_datetime(
        const std::chrono::system_clock::time_point &timepoint);
};
} // namespace ares

#endif // ARES_DATETIME_HPP
