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

TEST(logger_api, init) {
    Logger cut0("cut0", Logger::LOG_LEVEL_DBG);
    Logger cut1("cut1", Logger::LOG_LEVEL_INFO);
    Logger cut2("cut2", Logger::LOG_LEVEL_WARN);
    Logger cut3("cut3", Logger::LOG_LEVEL_ERROR);
    Logger cut4("cut4", Logger::LOG_LEVEL_CRITICAL);
    Logger cut5("cut5", Logger::LOG_LEVEL_OFF);

    ASSERT_EQ(cut0.get_log_level(), Logger::LOG_LEVEL_DBG);
    ASSERT_EQ(cut1.get_log_level(), Logger::LOG_LEVEL_INFO);
    ASSERT_EQ(cut2.get_log_level(), Logger::LOG_LEVEL_WARN);
    ASSERT_EQ(cut3.get_log_level(), Logger::LOG_LEVEL_ERROR);
    ASSERT_EQ(cut4.get_log_level(), Logger::LOG_LEVEL_CRITICAL);
    ASSERT_EQ(cut5.get_log_level(), Logger::LOG_LEVEL_OFF);
}

TEST(logger_api, logger_set_level) {
    Logger cut("cut", Logger::LOG_LEVEL_OFF);

    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_OFF);

    cut.set_log_level(Logger::LOG_LEVEL_DBG);
    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_DBG);

    cut.set_log_level(Logger::LOG_LEVEL_INFO);
    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_INFO);

    cut.set_log_level(Logger::LOG_LEVEL_WARN);
    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_WARN);

    cut.set_log_level(Logger::LOG_LEVEL_ERROR);
    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_ERROR);

    cut.set_log_level(Logger::LOG_LEVEL_CRITICAL);
    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_CRITICAL);

    cut.set_log_level(Logger::LOG_LEVEL_OFF);
    ASSERT_EQ(cut.get_log_level(), Logger::LOG_LEVEL_OFF);
}

#define Z_LOG_NOARGS(logger, expected, level, input)                           \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        logger.log(Logger::level, input);                                      \
        std::string output = testing::internal::GetCapturedStdout();           \
        ASSERT_EQ(output, expected);                                           \
    } while (false)

#define Z_LOG_ARGS(logger, expected, level, input, ...)                        \
    do {                                                                       \
        testing::internal::CaptureStdout();                                    \
        logger.log(Logger::level, input, __VA_ARGS__);                         \
        std::string output = testing::internal::GetCapturedStdout();           \
        ASSERT_EQ(output, expected);                                           \
    } while (false)

#define CHECK_OUTPUT(logger, expected, level, input, ...)                      \
    COND_CODE_0(IS_EMPTY(__VA_ARGS__),                                         \
                (Z_LOG_ARGS(logger, expected, level, input, __VA_ARGS__)),     \
                (Z_LOG_NOARGS(logger, expected, level, input)))

TEST(logger_api, log_various_messages_dbg) {
    Logger cut("cut", Logger::LOG_LEVEL_DBG);

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
    Logger cut("cut", Logger::LOG_LEVEL_INFO);

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
    Logger cut("cut", Logger::LOG_LEVEL_WARN);

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
    Logger cut("cut", Logger::LOG_LEVEL_ERROR);

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
    Logger cut("cut", Logger::LOG_LEVEL_CRITICAL);

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
    Logger cut("cut", Logger::LOG_LEVEL_OFF);

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
