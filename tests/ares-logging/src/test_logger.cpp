/**
 * @file test_logger.cpp
 *
 * @brief
 *
 * @date 5/11/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/logging/logger.hpp>
#include <ares/util.h>
#include <gtest/gtest.h>

#define RESET_COLOR "\033[0m"
#define DBG_COLOR   "\033[0m"
#define INF_COLOR   "\033[38;2;39;163;105m"
#define WARN_COLOR  "\033[38;2;163;115;76m"
#define ERR_COLOR   "\033[38;2;193;29;40m"
#define CRIT_COLOR  "\033[38;2;117;80;123m"

TEST(logger_api, init) {
    ares::Logger cut0("cut0", ares::Logger::LOG_LEVEL_DBG);
    ares::Logger cut1("cut1", ares::Logger::LOG_LEVEL_INFO);
    ares::Logger cut2("cut2", ares::Logger::LOG_LEVEL_WARN);
    ares::Logger cut3("cut3", ares::Logger::LOG_LEVEL_ERROR);
    ares::Logger cut4("cut4", ares::Logger::LOG_LEVEL_CRITICAL);
    ares::Logger cut5("cut5", ares::Logger::LOG_LEVEL_OFF);

    ASSERT_EQ(cut0.get_log_level(), ares::Logger::LOG_LEVEL_DBG);
    ASSERT_EQ(cut1.get_log_level(), ares::Logger::LOG_LEVEL_INFO);
    ASSERT_EQ(cut2.get_log_level(), ares::Logger::LOG_LEVEL_WARN);
    ASSERT_EQ(cut3.get_log_level(), ares::Logger::LOG_LEVEL_ERROR);
    ASSERT_EQ(cut4.get_log_level(), ares::Logger::LOG_LEVEL_CRITICAL);
    ASSERT_EQ(cut5.get_log_level(), ares::Logger::LOG_LEVEL_OFF);
}

TEST(logger_api, logger_set_level) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_OFF);

    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_OFF);

    cut.set_log_level(ares::Logger::LOG_LEVEL_DBG);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_DBG);

    cut.set_log_level(ares::Logger::LOG_LEVEL_INFO);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_INFO);

    cut.set_log_level(ares::Logger::LOG_LEVEL_WARN);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_WARN);

    cut.set_log_level(ares::Logger::LOG_LEVEL_ERROR);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_ERROR);

    cut.set_log_level(ares::Logger::LOG_LEVEL_CRITICAL);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_CRITICAL);

    cut.set_log_level(ares::Logger::LOG_LEVEL_OFF);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_OFF);
}

#define Z_LOG_NOARGS(logger, expected, level, input)                           \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        logger.log(ares::Logger::level, input);                                \
        std::string output = testing::internal::GetCapturedStdout();           \
        ASSERT_EQ(output, expected);                                           \
    } while (false)

#define Z_LOG_ARGS(logger, expected, level, input, ...)                        \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        logger.log(ares::Logger::level, input, __VA_ARGS__);                   \
        std::string output = testing::internal::GetCapturedStdout();           \
        ASSERT_EQ(output, expected);                                           \
    } while (false)

#define CHECK_OUTPUT(logger, expected, level, input, ...)                      \
    COND_CODE_0(IS_EMPTY(__VA_ARGS__),                                         \
                (Z_LOG_ARGS(logger, expected, level, input, __VA_ARGS__)),     \
                (Z_LOG_NOARGS(logger, expected, level, input)))

TEST(logger_api, log_various_messages_dbg) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_DBG);

    CHECK_OUTPUT(cut, "\033[0m[DBG]\033[0m cut: abcd\n", LOG_LEVEL_DBG, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;39;163;105m[INFO]\033[0m cut: abcd\n",
                 LOG_LEVEL_INFO, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd\n",
                 LOG_LEVEL_WARN, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd\n",
                 LOG_LEVEL_ERROR, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd\n",
                 LOG_LEVEL_CRITICAL, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(cut,
                 "\033[0m[DBG]\033[0m cut: abcd, foo: -12313213214454545 "
                 "1234605616436508569 -5\n",
                 LOG_LEVEL_DBG, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;39;163;105m[INFO]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_INFO, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_WARN, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_ERROR, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_CRITICAL, "abcd, %s: %lld %llu %hhd", foo, ll, ull,
                 i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);

    float f = -1.2356f;
    double d = -1.2356;

    CHECK_OUTPUT(
        cut, "\033[0m[DBG]\033[0m cut: abcd: -1.235600 100 -1.235600\n",
        LOG_LEVEL_DBG, "abcd: %f %d %f", static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;39;163;105m[INFO]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_INFO, "abcd: %f %d %f", static_cast<double>(f), 100,
                 d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_WARN, "abcd: %f %d %f", static_cast<double>(f), 100,
                 d);
    CHECK_OUTPUT(
        cut,
        "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd: -1.235600 100 -1.235600\n",
        LOG_LEVEL_ERROR, "abcd: %f %d %f", static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_CRITICAL, "abcd: %f %d %f", static_cast<double>(f),
                 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
}

TEST(logger_api, log_various_messages_info) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_INFO);

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;39;163;105m[INFO]\033[0m cut: abcd\n",
                 LOG_LEVEL_INFO, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd\n",
                 LOG_LEVEL_WARN, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd\n",
                 LOG_LEVEL_ERROR, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd\n",
                 LOG_LEVEL_CRITICAL, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;39;163;105m[INFO]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_INFO, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_WARN, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_ERROR, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_CRITICAL, "abcd, %s: %lld %llu %hhd", foo, ll, ull,
                 i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);

    float f = -1.2356f;
    double d = -1.2356;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;39;163;105m[INFO]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_INFO, "abcd: %f %d %f", static_cast<double>(f), 100,
                 d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_WARN, "abcd: %f %d %f", static_cast<double>(f), 100,
                 d);
    CHECK_OUTPUT(
        cut,
        "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd: -1.235600 100 -1.235600\n",
        LOG_LEVEL_ERROR, "abcd: %f %d %f", static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_CRITICAL, "abcd: %f %d %f", static_cast<double>(f),
                 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
}

TEST(logger_api, log_various_messages_warn) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_WARN);

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd\n",
                 LOG_LEVEL_WARN, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd\n",
                 LOG_LEVEL_ERROR, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd\n",
                 LOG_LEVEL_CRITICAL, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_WARN, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_ERROR, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_CRITICAL, "abcd, %s: %lld %llu %hhd", foo, ll, ull,
                 i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);

    float f = -1.2356f;
    double d = -1.2356;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;163;115;76m[WARN]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_WARN, "abcd: %f %d %f", static_cast<double>(f), 100,
                 d);
    CHECK_OUTPUT(
        cut,
        "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd: -1.235600 100 -1.235600\n",
        LOG_LEVEL_ERROR, "abcd: %f %d %f", static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_CRITICAL, "abcd: %f %d %f", static_cast<double>(f),
                 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
}

TEST(logger_api, log_various_messages_error) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_ERROR);

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd\n",
                 LOG_LEVEL_ERROR, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd\n",
                 LOG_LEVEL_CRITICAL, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_ERROR, "abcd, %s: %lld %llu %hhd", foo, ll, ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_CRITICAL, "abcd, %s: %lld %llu %hhd", foo, ll, ull,
                 i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);

    float f = -1.2356f;
    double d = -1.2356;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(
        cut,
        "\033[38;2;193;29;40m[ERR]\033[0m cut: abcd: -1.235600 100 -1.235600\n",
        LOG_LEVEL_ERROR, "abcd: %f %d %f", static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_CRITICAL, "abcd: %f %d %f", static_cast<double>(f),
                 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
}

TEST(logger_api, log_various_messages_critical) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_CRITICAL);

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_ERROR, "abcd");
    CHECK_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd\n",
                 LOG_LEVEL_CRITICAL, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_ERROR, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd, foo: "
                 "-12313213214454545 1234605616436508569 -5\n",
                 LOG_LEVEL_CRITICAL, "abcd, %s: %lld %llu %hhd", foo, ll, ull,
                 i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);

    float f = -1.2356f;
    double d = -1.2356;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_ERROR, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut,
                 "\033[38;2;117;80;123m[CRIT]\033[0m cut: abcd: -1.235600 100 "
                 "-1.235600\n",
                 LOG_LEVEL_CRITICAL, "abcd: %f %d %f", static_cast<double>(f),
                 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
}

TEST(logger_api, log_various_messages_off) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_OFF);

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_ERROR, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "abcd");
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd");

    const char *foo = "foo";
    unsigned long long ull = 0x1122334455667799;
    long long ll = -12313213214454545;
    int8_t i = -5;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_ERROR, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "abcd, %s: %lld %llu %hhd", foo,
                 ll, ull, i);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd, %s: %lld %llu %hhd", foo, ll,
                 ull, i);

    float f = -1.2356f;
    double d = -1.2356;

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_INFO, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_WARN, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_ERROR, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
    CHECK_OUTPUT(cut, "", LOG_LEVEL_OFF, "abcd: %f %d %f",
                 static_cast<double>(f), 100, d);
}

TEST(logger_api, log_runtime_switch) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_OFF);

    CHECK_OUTPUT(cut, "", LOG_LEVEL_DBG, "abcd");

    cut.set_log_level(ares::Logger::LOG_LEVEL_DBG);

    CHECK_OUTPUT(cut, "\033[0m[DBG]\033[0m cut: abcd\n", LOG_LEVEL_DBG, "abcd");
}

#define Z_LOG_HEX(logger, expected, level, msg, data, len)                     \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        logger.log_hexdump(ares::Logger::level, msg, data, len);               \
        std::string output = testing::internal::GetCapturedStdout();           \
        ASSERT_EQ(output, expected);                                           \
    } while (false)

#define CHECK_HEX_OUTPUT(logger, expected, level, msg, data, len)              \
    Z_LOG_HEX(logger, expected, level, msg, data, len)

TEST(logger_api, log_hexdump_output_dbg) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_DBG);
    std::vector<uint8_t> zero;
    std::vector<uint8_t> single = {0x00};
    std::vector<uint8_t> four = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> five = {0x00, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> eight = {0x00, 0x01, 0x02, 0x03,
                                  0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> nine = {0x00, 0x01, 0x02, 0x03, 0x04,
                                 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> v17 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    // No bytes
    CHECK_HEX_OUTPUT(cut, "\033[0m[DBG]\033[0m cut: foo\n\n", LOG_LEVEL_DBG,
                     "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n\n",
                     LOG_LEVEL_INFO, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n\n",
                     LOG_LEVEL_WARN, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n\n",
                     LOG_LEVEL_ERROR, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n\n",
                     LOG_LEVEL_CRITICAL, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", zero, 0);

    // 1 byte
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00                          |. \n",
                     LOG_LEVEL_DBG, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_INFO, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_WARN, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00                          |. \n",
                     LOG_LEVEL_ERROR, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", single, 1);

    // 4 bytes
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03                 |.... \n",
                     LOG_LEVEL_DBG, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_INFO, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_WARN, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03                 |.... \n",
                     LOG_LEVEL_ERROR, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_CRITICAL, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", four, 4);

    // 5 bytes
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_DBG, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_INFO, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_WARN, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_ERROR, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_CRITICAL, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", five, 5);

    // 8 bytes
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_DBG, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_INFO, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_WARN, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", eight, 8);

    // 9 bytes
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08                          |. \n",
                     LOG_LEVEL_DBG, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_INFO, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_WARN, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08                          |. \n",
                     LOG_LEVEL_ERROR, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", nine, 9);

    // 17 bytes
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_DBG, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_INFO, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_WARN, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 17);

    // 16 bytes (v17 with size set to 16)
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_DBG, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_INFO, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_WARN, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 16);

    // 17 bytes, size set to some larger number
    CHECK_HEX_OUTPUT(cut,
                     "\033[0m[DBG]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_DBG, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_INFO, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_WARN, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, (v17.size() + 32));
}

TEST(logger_api, log_hexdump_output_info) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_INFO);
    std::vector<uint8_t> zero;
    std::vector<uint8_t> single = {0x00};
    std::vector<uint8_t> four = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> five = {0x00, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> eight = {0x00, 0x01, 0x02, 0x03,
                                  0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> nine = {0x00, 0x01, 0x02, 0x03, 0x04,
                                 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> v17 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    // No bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n\n",
                     LOG_LEVEL_INFO, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n\n",
                     LOG_LEVEL_WARN, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n\n",
                     LOG_LEVEL_ERROR, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n\n",
                     LOG_LEVEL_CRITICAL, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", zero, 0);

    // 1 byte
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_INFO, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_WARN, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00                          |. \n",
                     LOG_LEVEL_ERROR, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", single, 1);

    // 4 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_INFO, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_WARN, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03                 |.... \n",
                     LOG_LEVEL_ERROR, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_CRITICAL, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", four, 4);

    // 5 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_INFO, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_WARN, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_ERROR, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_CRITICAL, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", five, 5);

    // 8 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_INFO, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_WARN, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", eight, 8);

    // 9 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_INFO, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_WARN, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08                          |. \n",
                     LOG_LEVEL_ERROR, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", nine, 9);

    // 17 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_INFO, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_WARN, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 17);

    // 16 bytes (v17 with size set to 16)
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_INFO, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_WARN, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 16);

    // 17 bytes, size set to some larger number
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;39;163;105m[INFO]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_INFO, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_WARN, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, (v17.size() + 32));
}

TEST(logger_api, log_hexdump_output_warn) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_WARN);
    std::vector<uint8_t> zero;
    std::vector<uint8_t> single = {0x00};
    std::vector<uint8_t> four = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> five = {0x00, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> eight = {0x00, 0x01, 0x02, 0x03,
                                  0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> nine = {0x00, 0x01, 0x02, 0x03, 0x04,
                                 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> v17 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    // No bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n\n",
                     LOG_LEVEL_WARN, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n\n",
                     LOG_LEVEL_ERROR, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n\n",
                     LOG_LEVEL_CRITICAL, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", zero, 0);

    // 1 byte
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_WARN, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00                          |. \n",
                     LOG_LEVEL_ERROR, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", single, 1);

    // 4 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_WARN, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03                 |.... \n",
                     LOG_LEVEL_ERROR, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_CRITICAL, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", four, 4);

    // 5 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_WARN, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_ERROR, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_CRITICAL, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", five, 5);

    // 8 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_WARN, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", eight, 8);

    // 9 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_WARN, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08                          |. \n",
                     LOG_LEVEL_ERROR, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", nine, 9);

    // 17 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_WARN, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 17);

    // 16 bytes (v17 with size set to 16)
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_WARN, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 16);

    // 17 bytes, size set to some larger number
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;163;115;76m[WARN]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_WARN, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, (v17.size() + 32));
}

TEST(logger_api, log_hexdump_output_error) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_ERROR);
    std::vector<uint8_t> zero;
    std::vector<uint8_t> single = {0x00};
    std::vector<uint8_t> four = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> five = {0x00, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> eight = {0x00, 0x01, 0x02, 0x03,
                                  0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> nine = {0x00, 0x01, 0x02, 0x03, 0x04,
                                 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> v17 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    // No bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n\n",
                     LOG_LEVEL_ERROR, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n\n",
                     LOG_LEVEL_CRITICAL, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", zero, 0);

    // 1 byte
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00                          |. \n",
                     LOG_LEVEL_ERROR, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", single, 1);

    // 4 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03                 |.... \n",
                     LOG_LEVEL_ERROR, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_CRITICAL, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", four, 4);

    // 5 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_ERROR, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_CRITICAL, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", five, 5);

    // 8 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", eight, 8);

    // 9 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08                          |. \n",
                     LOG_LEVEL_ERROR, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", nine, 9);

    // 17 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 17);

    // 16 bytes (v17 with size set to 16)
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_ERROR, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 16);

    // 17 bytes, size set to some larger number
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;193;29;40m[ERR]\033[0m cut: foo\n"
                     "           00 01 02 03   04 05 06 07   |.... ....\n"
                     "           08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "           10                          |. \n",
                     LOG_LEVEL_ERROR, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, (v17.size() + 32));
}

TEST(logger_api, log_hexdump_output_critical) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_CRITICAL);
    std::vector<uint8_t> zero;
    std::vector<uint8_t> single = {0x00};
    std::vector<uint8_t> four = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> five = {0x00, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> eight = {0x00, 0x01, 0x02, 0x03,
                                  0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> nine = {0x00, 0x01, 0x02, 0x03, 0x04,
                                 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> v17 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    // No bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n\n",
                     LOG_LEVEL_CRITICAL, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", zero, 0);

    // 1 byte
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", single, 1);

    // 4 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03                 |.... \n",
                     LOG_LEVEL_CRITICAL, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", four, 4);

    // 5 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04            |.... .\n",
                     LOG_LEVEL_CRITICAL, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", five, 5);

    // 8 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", eight, 8);

    // 9 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", nine, 9);

    // 17 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 17);

    // 16 bytes (v17 with size set to 16)
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n",
                     LOG_LEVEL_CRITICAL, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 16);

    // 17 bytes, size set to some larger number
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut,
                     "\033[38;2;117;80;123m[CRIT]\033[0m cut: foo\n"
                     "            00 01 02 03   04 05 06 07   |.... ....\n"
                     "            08 09 0a 0b   0c 0d 0e 0f   |.... ....\n"
                     "            10                          |. \n",
                     LOG_LEVEL_CRITICAL, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, (v17.size() + 32));
}

TEST(logger_api, log_hexdump_output_off) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_OFF);
    std::vector<uint8_t> zero;
    std::vector<uint8_t> single = {0x00};
    std::vector<uint8_t> four = {0x00, 0x01, 0x02, 0x03};
    std::vector<uint8_t> five = {0x00, 0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> eight = {0x00, 0x01, 0x02, 0x03,
                                  0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> nine = {0x00, 0x01, 0x02, 0x03, 0x04,
                                 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> v17 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    // No bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", zero, 0);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", zero, 0);

    // 1 byte
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", single, 1);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", single, 1);

    // 4 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", four, 4);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", four, 4);

    // 5 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", five, 5);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", five, 5);

    // 8 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", eight, 8);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", eight, 8);

    // 9 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", nine, 9);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", nine, 9);

    // 17 bytes
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", v17, 17);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 17);

    // 16 bytes (v17 with size set to 16)
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", v17, 16);
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, 16);

    // 17 bytes, size set to some larger number
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_INFO, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_WARN, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_ERROR, "foo", v17, (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_CRITICAL, "foo", v17,
                     (v17.size() + 32));
    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_OFF, "foo", v17, (v17.size() + 32));
}

TEST(logger_api, log_hexdump_runtime_switch) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_OFF);
    std::vector<uint8_t> zero;

    CHECK_HEX_OUTPUT(cut, "", LOG_LEVEL_DBG, "foo", zero, 0);

    cut.set_log_level(ares::Logger::LOG_LEVEL_DBG);

    CHECK_HEX_OUTPUT(cut, "\033[0m[DBG]\033[0m cut: foo\n\n", LOG_LEVEL_DBG,
                     "foo", zero, 0);
}

TEST(logger_api, log_hexdump_hex_output) {
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_DBG);
    std::vector<uint8_t> single = {0x00};
    std::string expected = "\033[0m[DBG]\033[0m cut: foo\n"
                           "           00                          |. \n";

    char lsd = '0', msd = '0';
    for (uint16_t i = 0x00; i <= UINT8_MAX; i++) {
        constexpr size_t lsd_index = 35;
        constexpr size_t msd_index = 34;
        constexpr size_t printable_index = 63;

        if ((i % 16) >= 10) {
            lsd = static_cast<char>(static_cast<char>((i % 16) - 10) + 'a');
        } else {
            lsd = static_cast<char>(static_cast<char>(i % 16) + '0');
        }

        if ((i / 16) >= 10) {
            msd = static_cast<char>(static_cast<char>((i / 16) - 10) + 'a');
        } else {
            msd = static_cast<char>(static_cast<char>(i / 16) + '0');
        }

        expected[lsd_index] = lsd;
        expected[msd_index] = msd;
        single[0] = static_cast<uint8_t>(i);

        if (std::isprint(i)) {
            expected[printable_index] = static_cast<char>(i);
        } else {
            expected[printable_index] = '.';
        }

        CHECK_HEX_OUTPUT(cut, expected, LOG_LEVEL_DBG, "foo", single, 1);
    }
}

namespace {
class PythonMockLogger {
  public:
    long level_ = 0;
    std::string msg_dbg;
    std::string msg_info;
    std::string msg_warn;
    std::string msg_error;
    std::string msg_crit;

    void debug(const std::string &msg) { msg_dbg = msg; }
    void info(const std::string &msg) { msg_info = msg; }
    void warn(const std::string &msg) { msg_warn = msg; }
    void error(const std::string &msg) { msg_error = msg; }
    void crit(const std::string &msg) { msg_crit = msg; }

    void set_level(long level) { level_ = level; }
    [[nodiscard]] long level() const { return level_; }
};
} // namespace

TEST(logger_api, log_cb_init) {
    PythonMockLogger mock;
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_DBG);
    ares::LoggerCallbacks cb{
        .set_level = [&mock](long level) { mock.set_level(level); },
        .get_level = [&mock]() { return mock.level(); },
    };
    cut.register_logging_callbacks(cb);

    ASSERT_EQ(mock.level_, static_cast<long>(ares::Logger::LOG_LEVEL_DBG));
    cut.set_log_level(ares::Logger::LOG_LEVEL_OFF);
    ASSERT_EQ(mock.level_, static_cast<long>(ares::Logger::LOG_LEVEL_OFF));

    mock.level_ = static_cast<long>(ares::Logger::LOG_LEVEL_WARN);
    ASSERT_EQ(cut.get_log_level(), ares::Logger::LOG_LEVEL_WARN);
}

TEST(logger_api, log_cb_msg) {
    PythonMockLogger mock;
    ares::Logger cut("cut", ares::Logger::LOG_LEVEL_DBG);
    ares::LoggerCallbacks cb{
        .dbg = [&mock](const std::string &msg) { mock.debug(msg); },
        .info = [&mock](const std::string &msg) { mock.info(msg); },
        .warn = [&mock](const std::string &msg) { mock.warn(msg); },
        .error = [&mock](const std::string &msg) { mock.error(msg); },
        .critical = [&mock](const std::string &msg) { mock.crit(msg); },
    };
    cut.register_logging_callbacks(cb);

    const char *dbg_msg = "AAAA";
    const char *info_msg = "BBBB";
    const char *warn_msg = "CCCC";
    const char *error_msg = "DDDD";
    const char *crit_msg = "EEEE";

    // Messages should get passed through no matter what
    cut.log(ares::Logger::LOG_LEVEL_DBG, "%s", dbg_msg);
    cut.log(ares::Logger::LOG_LEVEL_INFO, "%s", info_msg);
    cut.log(ares::Logger::LOG_LEVEL_WARN, "%s", warn_msg);
    cut.log(ares::Logger::LOG_LEVEL_ERROR, "%s", error_msg);
    cut.log(ares::Logger::LOG_LEVEL_CRITICAL, "%s", crit_msg);

    ASSERT_EQ(mock.msg_dbg, dbg_msg);
    ASSERT_EQ(mock.msg_info, info_msg);
    ASSERT_EQ(mock.msg_warn, warn_msg);
    ASSERT_EQ(mock.msg_error, error_msg);
    ASSERT_EQ(mock.msg_crit, crit_msg);
}
