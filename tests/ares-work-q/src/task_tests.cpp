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

TEST(task_api, task_construct_error) {
    ASSERT_THROW(ares::Task<void()> cut(nullptr), ares::TaskException);
}

TEST(task_api, task_get_result) {
    ares::Task<int()> cut0([]() { return 0xDEADBEEF; });
    ares::Task<int()> cut1([]() {
        throw std::runtime_error("foo");
        return 0;
    });
    ares::Task<void()> cut2([]() {});

    int ret;

    ASSERT_THROW(ret = cut0.get(), ares::TaskException);
    ASSERT_THROW(ret = cut1.get(), ares::TaskException);
    ASSERT_THROW(cut2.get(), ares::TaskException);

    cut0.start();
    cut1.start();
    cut2.start();

    ASSERT_THROW(ret = cut0.get(), ares::TaskException);
    ASSERT_THROW(ret = cut1.get(), ares::TaskException);
    ASSERT_THROW(cut2.get(), ares::TaskException);

    cut0.join();
    cut1.join();
    cut2.join();

    ASSERT_NO_THROW(ret = cut0.get());
    ASSERT_EQ(ret, 0xDEADBEEF);
    ASSERT_THROW(ret = cut0.get(), ares::TaskException);

    ASSERT_THROW(ret = cut1.get(), std::runtime_error);

    ASSERT_NO_THROW(cut2.get());
}
