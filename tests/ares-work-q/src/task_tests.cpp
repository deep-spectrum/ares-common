/**
 * @file task_tests.cpp
 *
 * @brief
 *
 * @date 5/13/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/synchronization/semaphore.hpp>
#include <ares/work-q/task.hpp>
#include <chrono>
#include <future>
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
    ASSERT_EQ(cut.get_state(), decltype(cut)::READY);

    cut.start();

    ASSERT_EQ(cut.set_essential(false), -EBUSY);
    ASSERT_EQ(cut.set_name("foo"), -EBUSY);
    ASSERT_NE(cut.get_id(), dummy.get_id());
    ASSERT_EQ(cut.get_state(), decltype(cut)::RUNNING);

    cut.join();
    EXPECT_EQ(cut.get_state(), decltype(cut)::JOINED);
}

TEST(task_api, task_construct_error) {
    ASSERT_THROW(ares::Task<void()> cut(nullptr), ares::TaskException);
}

TEST(task_api, task_get_result) {
    ares::Task<int()> cut0([]() { return 0xDEADBEEF; });
    ares::Task<int()> cut1([]() {
        throw std::runtime_error("foo");
        // ReSharper disable once CppDFAUnreachableCode
        return 0;
    });
    ares::Task<void()> cut2([]() {});

    int ret;

    // Task should be started first
    ASSERT_THROW(ret = cut0.get(), ares::TaskException);
    ASSERT_THROW(ret = cut1.get(), ares::TaskException);
    ASSERT_THROW(cut2.get(), ares::TaskException);

    cut0.start();
    cut1.start();
    cut2.start();

    // Task should be joined first
    ASSERT_THROW(ret = cut0.get(), ares::TaskException);
    ASSERT_THROW(ret = cut1.get(), ares::TaskException);
    ASSERT_THROW(cut2.get(), ares::TaskException);

    cut0.join();
    cut1.join();
    cut2.join();

    ASSERT_EQ(cut0.get_state(), decltype(cut0)::JOINED);

    // The result should be available now
    ASSERT_NO_THROW(ret = cut0.get());
    ASSERT_EQ(ret, 0xDEADBEEF);

    ASSERT_THROW(ret = cut1.get(), std::runtime_error);

    ASSERT_NO_THROW(cut2.get());

    // Reading the result should reset things back to the original state
    ASSERT_THROW(ret = cut0.get(), ares::TaskException);
    ASSERT_THROW(ret = cut1.get(), ares::TaskException);
    ASSERT_THROW(cut2.get(), ares::TaskException);
}

constexpr uint32_t val0 = 0xAAAA;
constexpr uint32_t val1 = 0xBBBB;
constexpr uint32_t val2 = 0xCCCC;

static void wait_until_ready(ares::semaphore<> &sem, const uint32_t value0,
                             const uint32_t value1, const uint32_t *value2) {
    EXPECT_NO_THROW(sem.take(5s));
    EXPECT_EQ(value0, val0);
    EXPECT_EQ(value1, val1);
    EXPECT_NE(value2, nullptr);
    EXPECT_EQ(*value2, val2);
}

TEST(task_api, task_start) {
    ares::Task<void(ares::semaphore<> &, uint32_t, uint32_t, const uint32_t *)>
        cut(wait_until_ready);
    ares::semaphore<> sem(0);
    std::thread dummy;
    const uint32_t *val2_ptr = &val2;

    // Can we start the task?
    EXPECT_NO_THROW(cut.start(std::ref(sem), val0, val1, val2_ptr));

    std::this_thread::sleep_for(500ms);
    std::thread::id id1 = cut.get_id();
    // Can the task create a new thread?
    EXPECT_NE(id1, dummy.get_id());
    EXPECT_NE(id1, std::this_thread::get_id());

    // Expect the task to throw an error while running
    EXPECT_THROW(cut.start(std::ref(sem), val0, val1, val2_ptr),
                 ares::TaskException);

    sem.give();

    cut.join();

    // Spawn a new task thread. This should spawn a new thread
    EXPECT_NO_THROW(cut.start(std::ref(sem), val0, val1, val2_ptr));
    // The task will reuse the old thread that was joined earlier. This will
    // keep the old thread ID.
    sem.give();
}

static void run_task(ares::Task<void(ares::semaphore<> &, uint32_t, uint32_t,
                                     const uint32_t *)> &cut,
                     ares::semaphore<> &sem, uint32_t value1, uint32_t value2,
                     const uint32_t *value3) {
    EXPECT_NO_THROW(cut.run(std::ref(sem), value1, value2, value3));

    std::this_thread::sleep_for(100ms);

    EXPECT_NO_THROW(cut.run(std::ref(sem), value1, value2, value3));
}

TEST(task_api, task_run) {
    ares::Task<void(ares::semaphore<> &, uint32_t, uint32_t, const uint32_t *)>
        cut(wait_until_ready);
    ares::semaphore<> sem(0);
    std::thread dummy, run_thread;
    const uint32_t *val2_ptr = &val2;

    run_thread = std::thread(run_task, std::ref(cut), std::ref(sem), val0, val1,
                             val2_ptr);
    // wait for other thread to initialize
    while (cut.get_state() != decltype(cut)::RUNNING) {
        std::this_thread::sleep_for(10ms);
    }
    EXPECT_NE(cut.get_id(), dummy.get_id());
    EXPECT_NE(cut.get_id(), std::this_thread::get_id());
    EXPECT_EQ(cut.get_id(), run_thread.get_id());

    sem.give();
    // If this gives trouble, replace with semaphore
    std::this_thread::sleep_for(20ms);
    EXPECT_EQ(cut.get_state(), decltype(cut)::JOINED);

    sem.give();
    run_thread.join();
}

static void sleepy_task(std::chrono::milliseconds sleep_time) {
    std::this_thread::sleep_for(sleep_time);
}

TEST(task_api, task_join) {
    auto future = std::async(std::launch::async, []() {
        ares::Task<void(std::chrono::milliseconds)> cut(sleepy_task);
        int ret;

        EXPECT_NO_THROW(ret = cut.join());
        EXPECT_EQ(ret, -EALREADY);
        EXPECT_EQ(cut.get_state(), decltype(cut)::READY);

        EXPECT_NO_THROW(cut.start(1s));
        EXPECT_EQ(cut.get_state(), decltype(cut)::RUNNING);

        EXPECT_NO_THROW(ret = cut.join());
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(cut.get_state(), decltype(cut)::JOINED);

        EXPECT_NO_THROW(cut.start(2s));

        EXPECT_NO_THROW(ret = cut.join(100ms));
        EXPECT_EQ(ret, -ETIMEDOUT);
        EXPECT_EQ(cut.get_state(), decltype(cut)::RUNNING);

        EXPECT_NO_THROW(ret = cut.join(4s));
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(cut.get_state(), decltype(cut)::JOINED);
    });

    EXPECT_EQ(future.wait_for(10s), std::future_status::ready);
}

TEST(task_api, task_destructor) {
    auto future = std::async(std::launch::async, []() {
        ares::Task<void(std::chrono::milliseconds)> cut(sleepy_task);

        cut.start(1s);

        // Test if task destructor actually joins the thread
    });

    EXPECT_EQ(future.wait_for(10s), std::future_status::ready);
    EXPECT_NO_THROW(future.get());
}
