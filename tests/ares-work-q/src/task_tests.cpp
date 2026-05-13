/**
 * @file task_tests.cpp
 *
 * @brief
 *
 * @date 5/13/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/work-q/task.hpp>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

void Empty() {}

TEST(task_api, basic_getters_setters) {
    ares::Task<void()> cut(Empty);

    ASSERT_FALSE(cut.get_essential());
    ASSERT_STREQ(cut.get_name(), "");

    // dummy thread that is not running
    std::thread dummy;
    ASSERT_EQ(cut.get_id(), dummy.get_id());

    ASSERT_EQ(cut.set_essential(true), 0);
    ASSERT_EQ(cut.set_name("cut"), 0);

    ASSERT_TRUE(cut.get_essential());
    ASSERT_STREQ(cut.get_name(), "cut");

    cut.start();

    ASSERT_EQ(cut.set_essential(false), -EBUSY);
    ASSERT_EQ(cut.set_name("foo"), -EBUSY);
    ASSERT_NE(cut.get_id(), dummy.get_id());

    // teardown handled by destructor...
}

void JoinHelper(int &value, int set_value,
                const std::chrono::milliseconds sleep_time) {
    std::this_thread::sleep_for(sleep_time);
    value = set_value;
}

TEST(task_api, task_construct_error) {
    ASSERT_THROW(ares::Task<void()> cut(nullptr), ares::TaskException);
}
