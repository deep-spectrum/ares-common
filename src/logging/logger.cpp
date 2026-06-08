//
// Created by tschmitz on 11/14/25.
//

#include <ares/logging/logger.hpp>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace ares {

constexpr const char *reset_color = "\033[0m";
constexpr const char *dbg_color = reset_color;
constexpr const char *inf_color = "\033[38;2;39;163;105m";
constexpr const char *wrn_color = "\033[38;2;163;115;76m";
constexpr const char *err_color = "\033[38;2;193;29;40m";
constexpr const char *crit_color = "\033[38;2;117;80;123m";

Logger::Logger(const char *name, LogLevel level) {
    _name = name;
    _level = level;
}

void Logger::set_log_level(LogLevel level) { _level = level; }

Logger::LogLevel Logger::get_log_level() const { return _level; }

void Logger::log(LogLevel level, const char *fmt, ...) const {
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    int len = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (len < 0) {
        return;
    }

    char *msg = new char[len + 1];
    vsnprintf(msg, len + 1, fmt, args);
    va_end(args);
    switch (level) {
    case LOG_LEVEL_DBG: {
        _log_dbg(msg);
        break;
    }
    case LOG_LEVEL_INFO: {
        _log_inf(msg);
        break;
    }
    case LOG_LEVEL_WARN: {
        _log_wrn(msg);
        break;
    }
    case LOG_LEVEL_ERROR: {
        _log_err(msg);
        break;
    }
    case LOG_LEVEL_CRITICAL: {
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

void Logger::log_hexdump(LogLevel level, const char *msg,
                         const std::vector<uint8_t> &buf, std::size_t bytes) {
    std::stringstream ss;
    ss << msg << "\n";
    size_t offset =
        (level == LOG_LEVEL_DBG || level == LOG_LEVEL_ERROR) ? 6 : 7;
    offset += strlen(_name) + 2;

    construct_hexdump(
        std::string_view{reinterpret_cast<const char *>(buf.data()),
                         std::min(bytes, buf.size())},
        offset, ss);

    switch (level) {
    case LOG_LEVEL_DBG: {
        _log_dbg(ss.str().c_str());
        break;
    }
    case LOG_LEVEL_INFO: {
        _log_inf(ss.str().c_str());
        break;
    }
    case LOG_LEVEL_WARN: {
        _log_wrn(ss.str().c_str());
        break;
    }
    case LOG_LEVEL_ERROR: {
        _log_err(ss.str().c_str());
        break;
    }
    case LOG_LEVEL_CRITICAL: {
        _log_crit(ss.str().c_str());
        break;
    }
    default:
        break;
    }
}

void Logger::_log_dbg(const char *msg) const {
    if (_level == LOG_LEVEL_DBG) {
        printf("%s[DBG]%s %s: %s\n", dbg_color, reset_color, _name, msg);
    }
}

void Logger::_log_inf(const char *msg) const {
    if (_level <= LOG_LEVEL_INFO) {
        printf("%s[INFO]%s %s: %s\n", inf_color, reset_color, _name, msg);
    }
}

void Logger::_log_wrn(const char *msg) const {
    if (_level <= LOG_LEVEL_WARN) {
        printf("%s[WARN]%s %s: %s\n", wrn_color, reset_color, _name, msg);
    }
}

void Logger::_log_err(const char *msg) const {
    if (_level <= LOG_LEVEL_ERROR) {
        printf("%s[ERR]%s %s: %s\n", err_color, reset_color, _name, msg);
    }
}

void Logger::_log_crit(const char *msg) const {
    if (_level <= LOG_LEVEL_CRITICAL) {
        printf("%s[CRIT]%s %s: %s\n", crit_color, reset_color, _name, msg);
    }
}
} // namespace ares
