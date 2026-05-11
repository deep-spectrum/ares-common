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
