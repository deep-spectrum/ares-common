//
// Created by tschmitz on 11/14/25.
//

#include <ares/logging/logger.hpp>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <map>
#include <sstream>

namespace ares {

constexpr const char *reset_color = "\033[0m";
constexpr const char *dbg_color = reset_color;
constexpr const char *inf_color = "\033[38;2;39;163;105m";
constexpr const char *wrn_color = "\033[38;2;163;115;76m";
constexpr const char *err_color = "\033[38;2;193;29;40m";
constexpr const char *crit_color = "\033[38;2;117;80;123m";

__attribute__((init_priority(
    101))) static std::map<std::string, std::shared_ptr<LoggerImpl>>
    loggers;

class LoggerImpl : std::enable_shared_from_this<LoggerImpl> {
  public:
    LoggerImpl(const char *name, Logger::LogLevel level);
    ~LoggerImpl();

    void set_log_level(Logger::LogLevel level);
    [[nodiscard]] Logger::LogLevel get_log_level() const;
    void log(Logger::LogLevel level, const char *fmt, va_list args) const;
    void log_hexdump(Logger::LogLevel level, const char *msg,
                     const std::vector<uint8_t> &buf, std::size_t bytes) const;
    void register_logging_callbacks(const LoggerCallbacks &cb);

    static std::shared_ptr<LoggerImpl> find_logger(const char *name,
                                                   Logger::LogLevel level);

  private:
    const char *_name;
    Logger::LogLevel _level;
    LoggerCallbacks _cb;

    void _log_dbg(const char *msg) const;
    void _log_inf(const char *msg) const;
    void _log_wrn(const char *msg) const;
    void _log_err(const char *msg) const;
    void _log_crit(const char *msg) const;
};

LoggerImpl::LoggerImpl(const char *name, Logger::LogLevel level) {
    _name = name;
    _level = level;
}

LoggerImpl::~LoggerImpl() { printf("Logger implementation destructor called"); }

void LoggerImpl::set_log_level(Logger::LogLevel level) {
    _level = level;

    if (_cb.set_level) {
        _cb.set_level(level);
    }
}

Logger::LogLevel LoggerImpl::get_log_level() const {
    if (_cb.get_level) {
        return static_cast<Logger::LogLevel>(_cb.get_level());
    }

    return _level;
}

void LoggerImpl::log(Logger::LogLevel level, const char *fmt,
                     va_list args) const {
    va_list copy;
    va_copy(copy, args);
    int len = vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);

    if (len <= 0) {
        return;
    }

    char *msg = new char[len + 1];
    vsnprintf(msg, len + 1, fmt, args);

    switch (level) {
    case Logger::LOG_LEVEL_DBG: {
        _log_dbg(msg);
        break;
    }
    case Logger::LOG_LEVEL_INFO: {
        _log_inf(msg);
        break;
    }
    case Logger::LOG_LEVEL_WARN: {
        _log_wrn(msg);
        break;
    }
    case Logger::LOG_LEVEL_ERROR: {
        _log_err(msg);
        break;
    }
    case Logger::LOG_LEVEL_CRITICAL: {
        _log_crit(msg);
        break;
    }
    default:
        break;
    }

    delete[] msg;
}

static void apply_padding(std::stringstream &ss, size_t pad) {
    ss << std::string(pad, ' ');
}

constexpr size_t bytes_per_line = 8;
constexpr size_t space_break = (bytes_per_line / 2) - 1;

static void dump_hex(size_t idx, const std::string_view data,
                     std::stringstream &ss) {
    for (size_t i = 0; i < bytes_per_line; i++) {
        if (i + idx < data.size()) {
            ss << std::hex << std::setw(2) << std::setfill('0')
               << (static_cast<int>(data[i + idx]) & 0xFF) << " ";
        } else {
            ss << "   ";
        }

        if (i == space_break) {
            ss << "  ";
        }
    }
}

static void dump_ascii(size_t idx, const std::string_view data,
                       std::stringstream &ss) {
    ss << "  |";
    size_t i;
    for (i = 0; i < bytes_per_line; i++) {
        if (i + idx < data.size()) {
            auto c = static_cast<uint8_t>(data[i + idx]);
            ss << (std::isprint(c) ? static_cast<char>(c) : '.');
        }

        if (i == space_break) {
            ss << " ";
        }
    }

    if (i + idx < data.size()) {
        ss << std::dec << "\n";
    }
}

static void construct_hexdump(const std::string_view data, size_t pad,
                              std::stringstream &ss) {
    for (size_t i = 0; i < data.size(); i += bytes_per_line) {
        apply_padding(ss, pad);
        dump_hex(i, data, ss);
        dump_ascii(i, data, ss);
    }
}

void LoggerImpl::log_hexdump(Logger::LogLevel level, const char *msg,
                             const std::vector<uint8_t> &buf,
                             std::size_t bytes) const {
    std::stringstream ss;
    ss << msg << "\n";
    size_t offset =
        (level == Logger::LOG_LEVEL_DBG || level == Logger::LOG_LEVEL_ERROR)
            ? 6
            : 7;
    offset += strlen(_name) + 2;

    construct_hexdump(
        std::string_view{reinterpret_cast<const char *>(buf.data()),
                         std::min(bytes, buf.size())},
        offset, ss);

    switch (level) {
    case Logger::LOG_LEVEL_DBG: {
        _log_dbg(ss.str().c_str());
        break;
    }
    case Logger::LOG_LEVEL_INFO: {
        _log_inf(ss.str().c_str());
        break;
    }
    case Logger::LOG_LEVEL_WARN: {
        _log_wrn(ss.str().c_str());
        break;
    }
    case Logger::LOG_LEVEL_ERROR: {
        _log_err(ss.str().c_str());
        break;
    }
    case Logger::LOG_LEVEL_CRITICAL: {
        _log_crit(ss.str().c_str());
        break;
    }
    default:
        break;
    }
}

void LoggerImpl::register_logging_callbacks(const LoggerCallbacks &cb) {
    _cb = cb;

    if (_cb.set_level) {
        _cb.set_level(_level);
    }
}

std::shared_ptr<LoggerImpl> LoggerImpl::find_logger(const char *name,
                                                    Logger::LogLevel level) {
    auto logger_it = loggers.find(name);

    if (logger_it == loggers.end()) {
        loggers[name] = std::make_shared<LoggerImpl>(name, level);
    }

    return loggers[name];
}

void LoggerImpl::_log_dbg(const char *msg) const {
    if (_cb.dbg) {
        _cb.dbg(msg);
        return;
    }

    if (_level == Logger::LOG_LEVEL_DBG) {
        printf("%s[DBG]%s %s: %s\n", dbg_color, reset_color, _name, msg);
    }
}

void LoggerImpl::_log_inf(const char *msg) const {
    if (_cb.info) {
        _cb.info(msg);
        return;
    }

    if (_level <= Logger::LOG_LEVEL_INFO) {
        printf("%s[INFO]%s %s: %s\n", inf_color, reset_color, _name, msg);
    }
}

void LoggerImpl::_log_wrn(const char *msg) const {
    if (_cb.warn) {
        _cb.warn(msg);
        return;
    }

    if (_level <= Logger::LOG_LEVEL_WARN) {
        printf("%s[WARN]%s %s: %s\n", wrn_color, reset_color, _name, msg);
    }
}

void LoggerImpl::_log_err(const char *msg) const {
    if (_cb.error) {
        _cb.error(msg);
        return;
    }

    if (_level <= Logger::LOG_LEVEL_ERROR) {
        printf("%s[ERR]%s %s: %s\n", err_color, reset_color, _name, msg);
    }
}

void LoggerImpl::_log_crit(const char *msg) const {
    if (_cb.critical) {
        _cb.critical(msg);
        return;
    }

    if (_level <= Logger::LOG_LEVEL_CRITICAL) {
        printf("%s[CRIT]%s %s: %s\n", crit_color, reset_color, _name, msg);
    }
}

Logger::Logger(const char *name, LogLevel level) {
    impl = LoggerImpl::find_logger(name, level);
}

void Logger::set_log_level(LogLevel level) const { impl->set_log_level(level); }

Logger::LogLevel Logger::get_log_level() const { return impl->get_log_level(); }

void Logger::log(LogLevel level, const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    impl->log(level, fmt, args);
    va_end(args);
}

void Logger::log_hexdump(LogLevel level, const char *msg,
                         const std::vector<uint8_t> &buf,
                         std::size_t bytes) const {
    impl->log_hexdump(level, msg, buf, bytes);
}

void Logger::register_logging_callbacks(const LoggerCallbacks &cb) const {
    impl->register_logging_callbacks(cb);
}
} // namespace ares
