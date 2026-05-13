/**
 * @file test_logging_macros.cpp
 *
 * @brief
 *
 * @date 5/12/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

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
