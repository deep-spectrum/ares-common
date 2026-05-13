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
