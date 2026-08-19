/**
 * @file test_logging_macros.cpp
 *
 * @brief
 *
 * @date 5/12/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#define Z_FILE_SCOPE_LOGGERS_DISABLE
#include <ares/logging/log.hpp>
#include <gtest/gtest.h>

TEST(logger_api, register_macro_default_level) {
    LOG_MODULE_REGISTER(default_level);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, DEFAULT_LOG_LEVEL);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, DEFAULT_LOG_LEVEL);
    ASSERT_EQ(LOG_MODULE_NAME, "default_level");
}

TEST(logger_api, register_macro_defined_level) {
    LOG_MODULE_REGISTER(defined_level, LOG_LEVEL_ERROR);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_ERROR);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_ERROR);
    ASSERT_EQ(LOG_MODULE_NAME, "defined_level");
}

TEST(logger_api, save_and_restore) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_DBG);

    // This should not override since the original level
    // was at the least restrictive level
    SAVE_LOG_LEVEL_AND_OVERRIDE(LOG_LEVEL_OFF);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_DBG);

    // This should have forced the change
    SAVE_LOG_LEVEL_AND_FORCE(LOG_LEVEL_OFF);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_OFF);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_DBG);

    // This should restore things back to the saved level
    RESTORE_LOG_LEVEL();

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_DBG);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_DBG);

    // This should override without saving
    SET_LOG_LEVEL(LOG_LEVEL_OFF);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_OFF);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_DBG);

    // This should override regardless now
    SAVE_LOG_LEVEL_AND_OVERRIDE(LOG_LEVEL_INFO);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_INFO);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_OFF);

    // This should restore back to the set level from SET_LOG_LEVEL()
    RESTORE_LOG_LEVEL();

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_OFF);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_OFF);

    // Middle of the road. More restrictive levels should not override, but less
    // restrictive should override
    SET_LOG_LEVEL(LOG_LEVEL_WARN);

    SAVE_LOG_LEVEL_AND_OVERRIDE(LOG_LEVEL_ERROR);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_WARN);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_WARN);

    SAVE_LOG_LEVEL_AND_OVERRIDE(LOG_LEVEL_INFO);

    ASSERT_EQ(LOG_MODULE_CURRENT_LEVEL, ares::Logger::LOG_LEVEL_INFO);
    ASSERT_EQ(LOG_MODULE_SAVED_LEVEL, ares::Logger::LOG_LEVEL_WARN);
}

#define CHECK_OUTPUT(LOG_FUNC, expected, input, ...)                           \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        LOG_FUNC(input, __VA_ARGS__);                                          \
        std::string output = testing::internal::GetCapturedStdout();           \
        EXPECT_EQ(output, expected);                                           \
    } while (false)

TEST(logger_api, log_dbg) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);

    CHECK_OUTPUT(LOG_DBG, "\033[0m[DBG]\033[0m test: \n", "");
    CHECK_OUTPUT(LOG_DBG, "\033[0m[DBG]\033[0m test: abcd\n", "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(LOG_DBG,
                 "\033[0m[DBG]\033[0m test: abcd, foo: -12313213214454545 "
                 "1234605616436508569 -5\n",
                 "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
}

TEST(logger_api, log_info) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);

    CHECK_OUTPUT(LOG_INF, "\033[38;2;39;163;105m[INFO]\033[0m test: \n", "");
    CHECK_OUTPUT(LOG_INF, "\033[38;2;39;163;105m[INFO]\033[0m test: abcd\n",
                 "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(LOG_INF,
                 "\033[38;2;39;163;105m[INFO]\033[0m test: abcd, foo: "
                 "-12313213214454545 "
                 "1234605616436508569 -5\n",
                 "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
}

TEST(logger_api, log_warn) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);

    CHECK_OUTPUT(LOG_WRN, "\033[38;2;163;115;76m[WARN]\033[0m test: \n", "");
    CHECK_OUTPUT(LOG_WRN, "\033[38;2;163;115;76m[WARN]\033[0m test: abcd\n",
                 "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(LOG_WRN,
                 "\033[38;2;163;115;76m[WARN]\033[0m test: abcd, foo: "
                 "-12313213214454545 "
                 "1234605616436508569 -5\n",
                 "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
}

TEST(logger_api, log_error) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);

    CHECK_OUTPUT(LOG_ERR, "\033[38;2;193;29;40m[ERR]\033[0m test: \n", "");
    CHECK_OUTPUT(LOG_ERR, "\033[38;2;193;29;40m[ERR]\033[0m test: abcd\n",
                 "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(
        LOG_ERR,
        "\033[38;2;193;29;40m[ERR]\033[0m test: abcd, foo: -12313213214454545 "
        "1234605616436508569 -5\n",
        "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
}

TEST(logger_api, log_crit) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);

    CHECK_OUTPUT(LOG_CRIT, "\033[38;2;117;80;123m[CRIT]\033[0m test: \n", "");
    CHECK_OUTPUT(LOG_CRIT, "\033[38;2;117;80;123m[CRIT]\033[0m test: abcd\n",
                 "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(LOG_CRIT,
                 "\033[38;2;117;80;123m[CRIT]\033[0m test: abcd, foo: "
                 "-12313213214454545 "
                 "1234605616436508569 -5\n",
                 "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
}

#define CHECK_LOG_HEX(LOG_FUNC, expected, msg, data, len)                      \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        LOG_FUNC(data, len, msg);                                              \
        std::string output = testing::internal::GetCapturedStdout();           \
        EXPECT_EQ(output, expected);                                           \
    } while (false)

TEST(logger_api, log_hexdump_dbg) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0x03,
                                 0x04, 0x05, 0x06, 0x07};

    CHECK_LOG_HEX(LOG_DBG_HEXDUMP,
                  "\033[0m[DBG]\033[0m test: foo\n"
                  "            00 01 02 03   04 05 06 07   |.... ....\n",
                  "foo", data, data.size());
}

TEST(logger_api, log_hexdump_info) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0x03,
                                 0x04, 0x05, 0x06, 0x07};

    CHECK_LOG_HEX(LOG_INF_HEXDUMP,
                  "\033[38;2;39;163;105m[INFO]\033[0m test: foo\n"
                  "             00 01 02 03   04 05 06 07   |.... ....\n",
                  "foo", data, data.size());
}

TEST(logger_api, log_hexdump_warn) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0x03,
                                 0x04, 0x05, 0x06, 0x07};

    CHECK_LOG_HEX(LOG_WRN_HEXDUMP,
                  "\033[38;2;163;115;76m[WARN]\033[0m test: foo\n"
                  "             00 01 02 03   04 05 06 07   |.... ....\n",
                  "foo", data, data.size());
}

TEST(logger_api, log_hexdump_error) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0x03,
                                 0x04, 0x05, 0x06, 0x07};

    CHECK_LOG_HEX(LOG_ERR_HEXDUMP,
                  "\033[38;2;193;29;40m[ERR]\033[0m test: foo\n"
                  "            00 01 02 03   04 05 06 07   |.... ....\n",
                  "foo", data, data.size());
}

TEST(logger_api, log_hexdump_crit) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0x03,
                                 0x04, 0x05, 0x06, 0x07};

    CHECK_LOG_HEX(LOG_CRIT_HEXDUMP,
                  "\033[38;2;117;80;123m[CRIT]\033[0m test: foo\n"
                  "             00 01 02 03   04 05 06 07   |.... ....\n",
                  "foo", data, data.size());
}

const char *dbg_msg = "AAAA";
const char *info_msg = "BBBB";
const char *warn_msg = "CCCC";
const char *error_msg = "DDDD";
const char *crit_msg = "EEEE";
long level = 0;

void dbg(const std::string &msg) { EXPECT_EQ(msg, dbg_msg); }

void info(const std::string &msg) { EXPECT_EQ(msg, info_msg); }

void warning(const std::string &msg) { EXPECT_EQ(msg, warn_msg); }

void error(const std::string &msg) { EXPECT_EQ(msg, error_msg); }

void critical(const std::string &msg) { EXPECT_EQ(msg, crit_msg); }

void set_level(long new_level) { level = new_level; }

long get_level() { return level; }

TEST(logger_api, log_register_callbacks) {
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    LOG_MODULE_REGISTER_CALLBACKS(dbg, info, warning, error, critical,
                                  set_level, get_level);

    LOG_DBG("%s", dbg_msg);
    LOG_INF("%s", info_msg);
    LOG_WRN("%s", warn_msg);
    LOG_ERR("%s", error_msg);
    LOG_CRIT("%s", crit_msg);

    EXPECT_EQ(level, static_cast<long>(ares::Logger::LOG_LEVEL_DBG));

    SAVE_LOG_LEVEL_AND_FORCE(LOG_LEVEL_CRITICAL);
    EXPECT_EQ(level, static_cast<long>(ares::Logger::LOG_LEVEL_CRITICAL));
    RESTORE_LOG_LEVEL();
    EXPECT_EQ(level, static_cast<long>(ares::Logger::LOG_LEVEL_DBG));
}
