/**
 * @file log.hpp
 *
 * @brief Logger helper macros.
 *
 * @date 11/17/2025
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_LOG_HPP
#define ARES_COMMON_LOG_HPP

#include <ares/logging/logger.hpp>
#include <ares/util.h>

/**
 * @cond doxygen_suppress
 */
#define Z_LOG_LEVEL_DBG  0
#define Z_LOG_LEVEL_INF  1
#define Z_LOG_LEVEL_WRN  2
#define Z_LOG_LEVEL_ERR  3
#define Z_LOG_LEVEL_CRIT 4
#define Z_LOG_LEVEL_OFF  5

#if LOG_LEVEL == Z_LOG_LEVEL_DBG
#define DEFAULT_LOG_LEVEL ares::Logger::LogLevel::LOG_LEVEL_DBG
#elif LOG_LEVEL == Z_LOG_LEVEL_INF
#define DEFAULT_LOG_LEVEL ares::Logger::LogLevel::LOG_LEVEL_INFO
#elif LOG_LEVEL == Z_LOG_LEVEL_WRN
#define DEFAULT_LOG_LEVEL ares::Logger::LogLevel::LOG_LEVEL_WARN
#elif LOG_LEVEL == Z_LOG_LEVEL_ERR
#define DEFAULT_LOG_LEVEL ares::Logger::LogLevel::LOG_LEVEL_ERROR
#elif LOG_LEVEL == Z_LOG_LEVEL_CRIT
#define DEFAULT_LOG_LEVEL ares::Logger::LogLevel::LOG_LEVEL_CRITICAL
#else
#define DEFAULT_LOG_LEVEL ares::Logger::LogLevel::LOG_LEVEL_OFF
#endif

#define Z_REGISTER_LOGGER_DEFAULT(name_)                                       \
    static const char *__name__ = #name_;                                      \
    __attribute__((init_priority(102))) static ares::Logger __logger__(        \
        __name__, DEFAULT_LOG_LEVEL);                                          \
    static ares::Logger::LogLevel __saved_level__ = DEFAULT_LOG_LEVEL;
#define Z_REGISTER_LOGGER(name_, level_)                                       \
    static const char *__name__ = #name_;                                      \
    __attribute__((init_priority(102))) static ares::Logger __logger__(        \
        __name__, ares::Logger::LogLevel::level_);                             \
    static ares::Logger::LogLevel __saved_level__ =                            \
        ares::Logger::LogLevel::level_;
/**
 * @endcond
 */

/**
 * Registers a module specific logger. This takes in the module name and
 * optionally, the logging level of the module.
 * @param name_ The module name.
 * @param level_ (Optional) The default logging level of the module.
 */
#define LOG_MODULE_REGISTER(name_, level_...)                                  \
    COND_CODE_0(IS_EMPTY(level_), (Z_REGISTER_LOGGER(name_, level_)),          \
                (Z_REGISTER_LOGGER_DEFAULT(name_)))

/**
 * The module name registered with @ref LOG_MODULE_REGISTER
 */
#define LOG_MODULE_NAME __name__

/**
 * The current logging level of the module.
 */
#define LOG_MODULE_CURRENT_LEVEL __logger__.get_log_level()

/**
 * The saved logging level of the module.
 */
#define LOG_MODULE_SAVED_LEVEL __saved_level__

/**
 * @brief Writes a DEBUG level message to the log.
 *
 * @param[in] msg_ A string optionally containing printf valid conversion
 * specifier, followed by as many values as specifiers.
 */
#define LOG_DBG(msg_, ...)                                                     \
    COND_CODE_0(IS_EMPTY(__VA_ARGS__),                                         \
                (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_DBG, msg_,   \
                                __VA_ARGS__)),                                 \
                (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_DBG, msg_)))

/**
 * @brief Writes an INFO level message to the log.
 *
 * @param[in] msg_ A string optionally containing printf valid conversion
 * specifier, followed by as many values as specifiers.
 */
#define LOG_INF(msg_, ...)                                                     \
    COND_CODE_0(                                                               \
        IS_EMPTY(__VA_ARGS__),                                                 \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_INFO, msg_,          \
                        __VA_ARGS__)),                                         \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_INFO, msg_)))

/**
 * @brief Writes a WARNING level message to the log.
 *
 * @param[in] msg_ A string optionally containing printf valid conversion
 * specifier, followed by as many values as specifiers.
 */
#define LOG_WRN(msg_, ...)                                                     \
    COND_CODE_0(                                                               \
        IS_EMPTY(__VA_ARGS__),                                                 \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_WARN, msg_,          \
                        __VA_ARGS__)),                                         \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_WARN, msg_)))

/**
 * @brief Writes an ERROR level message to the log.
 *
 * @param[in] msg_ A string optionally containing printf valid conversion
 * specifier, followed by as many values as specifiers.
 */
#define LOG_ERR(msg_, ...)                                                     \
    COND_CODE_0(                                                               \
        IS_EMPTY(__VA_ARGS__),                                                 \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_ERROR, msg_,         \
                        __VA_ARGS__)),                                         \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_ERROR, msg_)))

/**
 * @brief Writes a CRITICAL ERROR level message to the log.
 *
 * @param[in] msg_ A string optionally containing printf valid conversion
 * specifier, followed by as many values as specifiers.
 */
#define LOG_CRIT(msg_, ...)                                                    \
    COND_CODE_0(                                                               \
        IS_EMPTY(__VA_ARGS__),                                                 \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_CRITICAL, msg_,      \
                        __VA_ARGS__)),                                         \
        (__logger__.log(ares::Logger::LogLevel::LOG_LEVEL_CRITICAL, msg_)))

/**
 * @brief Writes a DEBUG level hex dump and message to the log.
 *
 * @param bytes_ The bytes as a std::vector<uint8_t> to dump as hex.
 * @param len_ The maximum number of bytes to dump.
 * @param msg_ A message associated with the hex dump
 */
#define LOG_DBG_HEXDUMP(bytes_, len_, msg_)                                    \
    __logger__.log_hexdump(ares::Logger::LogLevel::LOG_LEVEL_DBG, msg_,        \
                           bytes_, len_)

/**
 * @brief Writes an INFO level hex dump and message to the log.
 *
 * @param bytes_ The bytes as a std::vector<uint8_t> to dump as hex.
 * @param len_ The maximum number of bytes to dump.
 * @param msg_ A message associated with the hex dump
 */
#define LOG_INF_HEXDUMP(bytes_, len_, msg_)                                    \
    __logger__.log_hexdump(ares::Logger::LogLevel::LOG_LEVEL_INFO, msg_,       \
                           bytes_, len_)

/**
 * @brief Writes a WARNING level hex dump and message to the log.
 *
 * @param bytes_ The bytes as a std::vector<uint8_t> to dump as hex.
 * @param len_ The maximum number of bytes to dump.
 * @param msg_ A message associated with the hex dump
 */
#define LOG_WRN_HEXDUMP(bytes_, len_, msg_)                                    \
    __logger__.log_hexdump(ares::Logger::LogLevel::LOG_LEVEL_WARN, msg_,       \
                           bytes_, len_)

/**
 * @brief Writes a ERROR level hex dump and message to the log.
 *
 * @param bytes_ The bytes as a std::vector<uint8_t> to dump as hex.
 * @param len_ The maximum number of bytes to dump.
 * @param msg_ A message associated with the hex dump
 */
#define LOG_ERR_HEXDUMP(bytes_, len_, msg_)                                    \
    __logger__.log_hexdump(ares::Logger::LogLevel::LOG_LEVEL_ERROR, msg_,      \
                           bytes_, len_)

/**
 * @brief Writes a CRITICAL ERROR level hex dump and message to the log.
 *
 * @param bytes_ The bytes as a std::vector<uint8_t> to dump as hex.
 * @param len_ The maximum number of bytes to dump.
 * @param msg_ A message associated with the hex dump
 */
#define LOG_CRIT_HEXDUMP(bytes_, len_, msg_)                                   \
    __logger__.log_hexdump(ares::Logger::LogLevel::LOG_LEVEL_CRITICAL, msg_,   \
                           bytes_, len_)

/**
 * Saves the old logging level and sets the new logging level.
 * @param[in] new_level The new logging level of the module.
 *
 * @note This will only override the logging level if the old level is more
 * restrictive. If this is not desired behavior, use
 * @ref SAVE_LOG_LEVEL_AND_FORCE().
 */
#define SAVE_LOG_LEVEL_AND_OVERRIDE(new_level)                                 \
    do {                                                                       \
        __saved_level__ = __logger__.get_log_level();                          \
        if (ares::Logger::LogLevel::new_level < __saved_level__) {             \
            __logger__.set_log_level(ares::Logger::LogLevel::new_level);       \
        }                                                                      \
    } while (false)

/**
 * Saves the old logging level and forces the new logging level.
 * @param new_level The new logging level of the module.
 *
 * @note This will override the logging level no matter what. If you want to
 * avoid overriding the logging level if the current level is more permissive,
 * the see @ref SAVE_LOG_LEVEL_AND_OVERRIDE().
 */
#define SAVE_LOG_LEVEL_AND_FORCE(new_level)                                    \
    do {                                                                       \
        __saved_level__ = __logger__.get_log_level();                          \
        __logger__.set_log_level(ares::Logger::LogLevel::new_level);           \
    } while (false)

/**
 * Restores the logging level back to its saved old level.
 *
 * @see SAVE_LOG_LEVEL_AND_OVERRIDE
 * @see SAVE_LOG_LEVEL_AND_FORCE
 */
#define RESTORE_LOG_LEVEL()                                                    \
    do {                                                                       \
        __logger__.set_log_level(__saved_level__);                             \
    } while (false)

/**
 * Set the logging level of the module. This will not save the old logging
 * level.
 * @param new_level The new logging level of the module.
 */
#define SET_LOG_LEVEL(new_level)                                               \
    do {                                                                       \
        __logger__.set_log_level(ares::Logger::LogLevel::new_level);           \
    } while (false)

/**
 * Register redirect callbacks for the logger.
 *
 * @param debug_ Debug callback.
 * @param info_ Info callback.
 * @param warning_ Warning callback.
 * @param error_ Error callback.
 * @param critical_ Critical error callback.
 * @param set_level_ Set level callback.
 * @param get_level_ Get level callback.
 */
#define LOG_MODULE_REGISTER_CALLBACKS(debug_, info_, warning_, error_,         \
                                      critical_, set_level_, get_level_)       \
    do {                                                                       \
        ares::LoggerCallbacks cb = {                                           \
            .dbg = debug_,                                                     \
            .info = info_,                                                     \
            .warn = warning_,                                                  \
            .error = error_,                                                   \
            .critical = critical_,                                             \
            .set_level = set_level_,                                           \
            .get_level = get_level_,                                           \
        };                                                                     \
        __logger__.register_logging_callbacks(cb);                             \
    } while (false)

#endif // ARES_COMMON_LOG_HPP
