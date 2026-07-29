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
/**
 * @class DateTimeException
 * Exception class for conversion errors.
 */
class DateTimeException : std::exception {
  public:
    /**
     * Constructor.
     * @param msg The error message.
     */
    explicit DateTimeException(const char *msg) : _msg(msg) {}

    /**
     * What caused the exception.
     * @return The error message.
     */
    [[nodiscard]] const char *what() const noexcept override {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};

/**
 * @class DateTime
 * Datetime representation in C++.
 */
class DateTime {
  public:
    /**
     * Constructor.
     */
    DateTime() = default;

    /**
     * Constructor.
     * @param[in] timepoint The timepoint to extract the date and time from.
     */
    explicit DateTime(const std::chrono::system_clock::time_point &timepoint);

    /**
     * Constructor.
     * @param[in] year The year of the timepoint.
     * @param[in] month The month [1-12].
     * @param[in] day The day [1-31].
     * @param[in] hour The hour [0-23].
     * @param[in] minute The minute [0-59].
     * @param[in] second The second [0-60] (accounting for leap seconds).
     * @param[in] microsecond The microsecond.
     */
    explicit DateTime(int year, int month, int day, int hour, int minute,
                      int second, int microsecond = 0);

    /**
     * Year.
     * @return The year.
     */
    [[nodiscard]] int year() const;

    /**
     * Month.
     * @return The month.
     */
    [[nodiscard]] int month() const;

    /**
     * Day.
     * @return The day.
     */
    [[nodiscard]] int day() const;

    /**
     * Hour.
     * @return The hour.
     */
    [[nodiscard]] int hour() const;

    /**
     * Minute.
     * @return The minute.
     */
    [[nodiscard]] int minute() const;

    /**
     * Second.
     * @return The second.
     */
    [[nodiscard]] int second() const;

    /**
     * Microsecond.
     * @return The microsecond.
     */
    [[nodiscard]] int microsecond() const;

    /**
     * Get the timepoint representation of the datetime.
     * @return The time point representation.
     */
    [[nodiscard]] std::chrono::system_clock::time_point time_point() const;

    /**
     * I don't know why this shit isn't implicitly generated for me...
     * @param rhs Right hand side.
     * @return This object.
     */
    DateTime &operator=(const DateTime &rhs) = default;

  private:
    std::chrono::system_clock::time_point _timepoint{};
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
