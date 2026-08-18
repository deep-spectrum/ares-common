/**
 * @file logger.hpp
 *
 * @brief C++ logger implementation.
 *
 * @date 11/14/2025
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_LOGGER_HPP
#define ARES_COMMON_LOGGER_HPP

#include <ares/data-structures/sys/slist.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ares {
/**
 * @struct LoggerCallbacks
 * Custom callback handles for logging.
 */
struct LoggerCallbacks {
    /**
     * Debug message callback.
     * @param[in] msg Logging message.
     */
    std::function<void(const std::string &msg)> dbg = nullptr;

    /**
     * Info message callback.
     * @param[in] msg Logging message.
     */
    std::function<void(const std::string &msg)> info = nullptr;

    /**
     * Warning message callback.
     * @param[in] msg Logging message.
     */
    std::function<void(const std::string &msg)> warn = nullptr;

    /**
     * Error message callback.
     * @param[in] msg Logging message.
     */
    std::function<void(const std::string &msg)> error = nullptr;

    /**
     * Critical error message callback.
     * @param[in] msg Logging message.
     */
    std::function<void(const std::string &msg)> critical = nullptr;

    /**
     * Callback for setting the log message level.
     * @param[in] level The new logging level.
     *
     * @note This should rarely be used. This was primarily built for instances
     * where logging needs to be redirected to a Python logger.
     */
    std::function<void(long level)> set_level = nullptr;

    /**
     * Callback for getting the log message level.
     *
     * @note This should rarely be used. This was primarily built for instances
     * where logging needs to be redirected to a Python logger.
     */
    std::function<long()> get_level = nullptr;
};

/**
 * @class Logger
 * Implementation of C++ logger.
 *
 * @note If using a Python logger, it should be noted that `OFF` is considered
 * to be `logging.CRITICAL + 10`.
 */
class Logger {
  public:
    /**
     * Logging levels.
     */
    enum LogLevel : unsigned int {
        LOG_LEVEL_DBG = 10,      ///< Debug logging level.
        LOG_LEVEL_INFO = 20,     ///< Info logging level.
        LOG_LEVEL_WARN = 30,     ///< Warning logging level.
        LOG_LEVEL_ERROR = 40,    ///< Error logging level.
        LOG_LEVEL_CRITICAL = 50, ///< Critical logging level.
        LOG_LEVEL_OFF = 60,      ///< Logging turned off.
    };

    /**
     * Constructor.
     * @param[in] name The name of the logger.
     * @param[in] level The starting logging level.
     */
    explicit Logger(const char *name, LogLevel level);

    /**
     * Destructor.
     */
    ~Logger();

    /**
     * Sets the current logging level.
     * @param[in] level The new logging level.
     */
    void set_log_level(LogLevel level);

    /**
     * Retrieve the current logging level.
     * @return The current logging level.
     */
    [[nodiscard]] LogLevel get_log_level() const;

    /**
     * @brief Log a message with the given level.
     *
     * Logs the given message with the given  level. If the loggers level is set
     * higher than the given level, then the message will not be logged.
     *
     * @param[in] level The logging message type. Will do nothing if set to
     * `LOG_LEVEL_OFF`.
     * @param[in] fmt The format string for the log message.
     * @param[in] ... Additional parameters needed for the format string.
     */
    void log(LogLevel level, const char *fmt, ...) const;

    /**
     * @brief Log a message with the given level.
     *
     * Logs the given message with the given level. If the loggers level is set
     * higher than the given level, then the message will not be logged.
     *
     * @param[in] level The logging message type. Will do nothing if set to
     * `LOG_LEVEL_OFF`
     * @param[in] msg Persistent, raw string. Will be displayed for each dump.
     * @param[in] buf The data to be logged.
     * @param[in] bytes Length of the data to be logged (in bytes).
     */
    void log_hexdump(LogLevel level, const char *msg,
                     const std::vector<uint8_t> &buf, std::size_t bytes) const;

    /**
     * Register logging callbacks. This is primarily meant for redirecting the
     * messages to a different logger implementation.
     *
     * @param[in] cb The logger callbacks to register.
     */
    void register_logging_callbacks(const LoggerCallbacks &cb);

    friend Logger &get_logger_by_name(const char *name);

  private:
    const char *_name;
    LogLevel _level;
    LoggerCallbacks _cb;

    void _log_dbg(const char *msg) const;
    void _log_inf(const char *msg) const;
    void _log_wrn(const char *msg) const;
    void _log_err(const char *msg) const;
    void _log_crit(const char *msg) const;

    sys_snode_t node{};
    std::shared_ptr<sys_slist_t> list;
};

Logger &get_logger_by_name(const char *name);
} // namespace ares

#endif // ARES_COMMON_LOGGER_HPP