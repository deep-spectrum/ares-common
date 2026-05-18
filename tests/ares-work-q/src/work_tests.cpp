/**
 * @file work_tests.cpp
 *
 * @brief
 *
 * @date 5/14/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/synchronization/semaphore.hpp>
#include <ares/util.hpp>
#include <ares/work-q/work_q.hpp>
#include <atomic>
#include <gtest/gtest.h>
#include <memory>

using namespace std::chrono_literals;

constexpr size_t max_sem_cnt = 10;
constexpr int work_q_core = 1;

struct CounterWork {
    explicit CounterWork(
        const ares::work_handler_t &handler, const std::thread::id inc,
        const std::shared_ptr<ares::semaphore<max_sem_cnt>> &s_sem,
        const std::shared_ptr<std::atomic_int> &cnt)
        : count(cnt), inc_thread(inc), sync_sem(s_sem), work(handler) {}

    std::shared_ptr<std::atomic_int> count;
    std::thread::id inc_thread;
    std::atomic_int resubmits_left = 0;
    std::shared_ptr<ares::semaphore<max_sem_cnt>> sync_sem;
    std::shared_ptr<ares::semaphore<max_sem_cnt>> rel_sem = nullptr;
    ares::Work work;
};

static void counter_handler(ares::Work *work) {
    auto counter_work = ares::container_of(work, &CounterWork::work);

    if (counter_work->inc_thread == std::this_thread::get_id()) {
        ++(*counter_work->count);
    } else {
        --(*counter_work->count);
    }

    --counter_work->resubmits_left;
    if (counter_work->resubmits_left > 0) {
        EXPECT_EQ(ares::work_submit_to_queue(nullptr, work), 2);
    } else {
        counter_work->sync_sem->give();
    }
}

TEST(work, unstarted) {
    ares::WorkQ not_started_q;

    ares::Work work(counter_handler);
    EXPECT_EQ(work.work_busy_get(), 0);

    int rc = not_started_q.submit(&work);
    EXPECT_EQ(rc, -ENODEV);
}

TEST(work, null_queue) {
    ares::Work work(counter_handler);
    ASSERT_EQ(work.work_busy_get(), 0);

    int rc = ares::work_submit_to_queue(nullptr, &work);
    ASSERT_EQ(rc, -EINVAL);
}

TEST(work, simple_submit) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    CounterWork work(counter_handler, work_q.queue_thread_get(), sync_sem,
                     count);

    EXPECT_EQ(work.work.work_busy_get(), 0);
    EXPECT_FALSE(work.work.work_is_pending());

    int rc = work_q.submit(&work.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(work.work.work_busy_get(), ares::WORK_QUEUED);
    EXPECT_TRUE(work.work.work_is_pending());
    EXPECT_EQ(*count, 0);

    std::this_thread::sleep_for(1ms);
    EXPECT_EQ(*count, 1);
    EXPECT_EQ(work.work.work_busy_get(), 0);

    EXPECT_NO_THROW(sync_sem->take(ares::no_wait));
}

static void rel_handler(ares::Work *work) {
    auto cnt_work = ares::container_of(work, &CounterWork::work);
    cnt_work->rel_sem->take();
    counter_handler(work);
}

TEST(work, sync_queue) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto rel_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    CounterWork work(rel_handler, work_q.queue_thread_get(), sync_sem, count);
    work.rel_sem = rel_sem;

    EXPECT_EQ(work.work.work_busy_get(), 0);
    EXPECT_FALSE(work.work.work_is_pending());

    int rc = work_q.submit(&work.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(work.work.work_busy_get(), ares::WORK_QUEUED);

    EXPECT_EQ(*count, 0);

    std::this_thread::sleep_for(1ms);
    EXPECT_EQ(*count, 0);
    EXPECT_EQ(work.work.work_busy_get(), ares::WORK_RUNNING);

    rel_sem->give();
    EXPECT_EQ(*count, 0);

    EXPECT_NO_THROW(sync_sem->take());
    EXPECT_EQ(*count, 1);
}

TEST(work, reentrent_queue) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto rel_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q0, work_q1;
    work_q0.start(nullptr);
    work_q1.start(nullptr);

    CounterWork work(rel_handler, work_q0.queue_thread_get(), sync_sem, count);
    work.rel_sem = rel_sem;

    int rc = work_q0.submit(&work.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(*count, 0);

    std::this_thread::sleep_for(1ms);
    EXPECT_EQ(*count, 0);

    rc = work_q1.submit(&work.work);
    EXPECT_EQ(rc, 2);

    rel_sem->give();
    EXPECT_NO_THROW(sync_sem->take());
    EXPECT_EQ(*count, 1);

    rel_sem->give();
    EXPECT_NO_THROW(sync_sem->take());
    EXPECT_EQ(*count, 2);
}

// ReSharper disable once CppUseAuto
constexpr auto delay = 100ms;

static void delay_handler(ares::Work *work) {
    std::this_thread::sleep_for(delay);
    counter_handler(work);
}

bool PredicateCheckWorkBusyFlags(uint32_t val1, uint32_t val2, uint32_t val3) {
    return (val1 == val2) || (val1 == val3);
}

testing::AssertionResult PredicateCheckWorkBusy(const char *val1_expr,
                                                const char *val2_expr,
                                                const char *val3_expr,
                                                uint32_t val1, uint32_t val2,
                                                uint32_t val3) {
    if (PredicateCheckWorkBusyFlags(val1, val2, val3)) {
        return testing::AssertionSuccess();
    }

    return testing::AssertionFailure()
           << "Expected equality of these values:\n  " << val1_expr
           << "\n    Which is: " << val1 << "\n  " << val2_expr
           << "\n    Which is: " << val2 << "\n Or\n  " << val3_expr
           << "\n    Which is: " << val3;
}

TEST(work, queued_flush) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    CounterWork work0(delay_handler, work_q.queue_thread_get(), sync_sem,
                      count);
    CounterWork work1(delay_handler, work_q.queue_thread_get(), sync_sem,
                      count);

    int rc = work_q.submit(&work1.work);
    EXPECT_EQ(rc, 1);
    rc = work_q.submit(&work0.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(*count, 0);

    EXPECT_EQ(work0.work.work_busy_get(), ares::WORK_QUEUED);

    // Checking for both since we are at the mercy of the scheduler...
    EXPECT_PRED_FORMAT3(PredicateCheckWorkBusy, work1.work.work_busy_get(),
                        ares::WORK_QUEUED, ares::WORK_RUNNING);
    EXPECT_TRUE(work0.work.work_flush());
    EXPECT_FALSE(work1.work.work_flush());

    EXPECT_EQ(*count, 2);
    EXPECT_FALSE(work0.work.work_is_pending());
    EXPECT_FALSE(work1.work.work_is_pending());
    EXPECT_NO_THROW(sync_sem->take(ares::no_wait));

    EXPECT_FALSE(work0.work.work_is_pending());
    EXPECT_FALSE(work1.work.work_is_pending());
}

TEST(work, running_flush) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    CounterWork work(delay_handler, work_q.queue_thread_get(), sync_sem, count);

    int rc = work_q.submit(&work.work);
    EXPECT_EQ(work.work.work_busy_get(), ares::WORK_QUEUED);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(*count, 0);

    std::this_thread::sleep_for(1ms);

    EXPECT_EQ(work.work.work_busy_get(), ares::WORK_RUNNING);
    EXPECT_EQ(*count, 0);

    EXPECT_TRUE(work.work.work_flush());

    EXPECT_EQ(*count, 1);
    EXPECT_NO_THROW(sync_sem->take(ares::no_wait));
}

TEST(work, delayed_flush) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, queued_cancel) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto rel_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    CounterWork work(rel_handler, work_q.queue_thread_get(), sync_sem, count);
    work.rel_sem = rel_sem;
    // prevent blocking just in case
    rel_sem->give();

    int rc = work_q.submit(&work.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(*count, 0);

    EXPECT_FALSE(work.work.work_cancel());
    EXPECT_EQ(*count, 0);
}

TEST(work, queued_cancel_sync) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto rel_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    CounterWork work(rel_handler, work_q.queue_thread_get(), sync_sem, count);
    work.rel_sem = rel_sem;

    EXPECT_FALSE(work.work.work_cancel_sync());

    int rc = work_q.submit(&work.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(*count, 0);

    EXPECT_TRUE(work.work.work_cancel_sync());
    EXPECT_EQ(*count, 0);
}

TEST(work, delayed_cancel) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, delayed_cancel_sync) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, delayed_cancel_sync_wait) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

struct TestRunningCancelTimer {
    explicit TestRunningCancelTimer(
        const ares::work_handler_t &handler, const std::thread::id inc,
        const std::shared_ptr<ares::semaphore<max_sem_cnt>> &s_sem,
        const std::shared_ptr<std::atomic_int> &cnt)
        : work(handler, inc, s_sem, cnt) {}

    CounterWork work;
    int submit_rc = 0;
    int busy_rc = 0;
};

static void test_running_cancel_cb(TestRunningCancelTimer &ctx, ares::WorkQ &q,
                                   std::chrono::milliseconds delay_) {
    // change_thread_prio(HIGH);
    std::this_thread::sleep_for(delay_);

    ctx.busy_rc = ctx.work.work.work_busy_get();
    ctx.submit_rc = q.submit(&ctx.work.work);
    ctx.work.rel_sem->give();
}

TEST(work, running_cancel) {
    auto sync_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto rel_sem = std::make_shared<ares::semaphore<max_sem_cnt>>(0);
    auto count = std::make_shared<std::atomic_int>(0);

    // change_thread_prio(MED);

    ares::WorkQ work_q;
    work_q.start(nullptr);

    TestRunningCancelTimer ctx(rel_handler, work_q.queue_thread_get(), sync_sem,
                               count);
    ctx.work.rel_sem = rel_sem;

    int rc = work_q.submit(&ctx.work.work);
    EXPECT_EQ(rc, 1);
    EXPECT_EQ(*count, 0);

    std::this_thread::sleep_for(1ms);
    EXPECT_EQ(*count, 0);

    ctx.busy_rc = INT_MAX;
    ctx.submit_rc = INT_MAX;
    std::thread t(test_running_cancel_cb, std::ref(ctx), std::ref(work_q),
                  10ms);

    EXPECT_EQ(ctx.work.work.work_cancel(),
              ares::WORK_RUNNING | ares::WORK_CANCELING);
    ASSERT_EQ(*count, 0);

    EXPECT_TRUE(ctx.work.work.work_cancel_sync());

    EXPECT_NO_THROW(sync_sem->take(ares::no_wait));
    EXPECT_EQ(ctx.busy_rc, ares::WORK_RUNNING | ares::WORK_CANCELING);
    EXPECT_EQ(ctx.submit_rc, -EBUSY);

    EXPECT_EQ(ctx.work.work.work_busy_get(), 0);

    t.join();
}

TEST(work, running_cancel_sync) {}

TEST(work, drain_empty) {}

TEST(work, drain_wait) {}

TEST(work, plugged_drain) {}

TEST(work, basic_schedule) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, basic_schedule_running) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, immediate_schedule) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, basic_reschedule) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, immediate_reschedule) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, queue_no_yield) {}

TEST(work, system_queue) {}

TEST(work, system_schedule) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, system_reschedule) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}
