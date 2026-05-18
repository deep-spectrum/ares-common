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

using namespace std::chrono_literals;

struct CounterWork {
    enum operation {
        ADD,
        SUB,
    };

    explicit CounterWork(const ares::work_handler_t &handler)
        : sync_sem(0), rel_sem(0), work(handler) {}

    std::atomic_int count = 0;
    operation op = ADD;
    std::atomic_int resubmits_left = 0;
    ares::semaphore<> sync_sem;
    ares::semaphore<> rel_sem;
    ares::Work work;
};

static void counter_handler(ares::Work *work) {
    auto counter_work = ares::container_of(work, &CounterWork::work);

    switch (counter_work->op) {
    case CounterWork::ADD: {
        ++counter_work->count;
        break;
    }
    case CounterWork::SUB: {
        --counter_work->count;
        break;
    }
    default: {
        EXPECT_FALSE(true);
        break;
    }
    }

    --counter_work->resubmits_left;
    if (counter_work->resubmits_left > 0) {
        EXPECT_EQ(ares::work_submit_to_queue(nullptr, work), 2);
    } else {
        counter_work->sync_sem.give();
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
    CounterWork work(counter_handler);
    ares::WorkQ work_q;

    work_q.start(nullptr);

    ASSERT_EQ(work.work.work_busy_get(), 0);
    ASSERT_FALSE(work.work.work_is_pending());

    int rc = work_q.submit(&work.work);
    ASSERT_EQ(rc, 1);
    ASSERT_EQ(work.work.work_busy_get(), ares::WORK_QUEUED);
    ASSERT_TRUE(work.work.work_is_pending());
    ASSERT_EQ(work.count, 0);

    std::this_thread::sleep_for(1ms);
    ASSERT_EQ(work.count, 1);
    ASSERT_EQ(work.work.work_busy_get(), 0);

    ASSERT_NO_THROW(work.sync_sem.take(ares::no_wait));
}

static void rel_handler(ares::Work *work) {
    auto cnt_work = ares::container_of(work, &CounterWork::work);
    cnt_work->rel_sem.take();
    counter_handler(work);
}

TEST(work, sync_queue) {
    CounterWork work(rel_handler);
    ares::WorkQ work_q;

    work_q.start(nullptr);

    ASSERT_EQ(work.work.work_busy_get(), 0);
    ASSERT_FALSE(work.work.work_is_pending());

    int rc = work_q.submit(&work.work);
    ASSERT_EQ(rc, 1);
    ASSERT_EQ(work.work.work_busy_get(), ares::WORK_QUEUED);

    ASSERT_EQ(work.count, 0);

    std::this_thread::sleep_for(1ms);
    ASSERT_EQ(work.count, 0);
    ASSERT_EQ(work.work.work_busy_get(), ares::WORK_RUNNING);

    work.rel_sem.give();
    ASSERT_EQ(work.count, 0);

    ASSERT_NO_THROW(work.sync_sem.take());
    ASSERT_EQ(work.count, 1);
}

TEST(work, reentrent_queue) {}

TEST(work, queued_flush) {}

TEST(work, running_flush) {}

TEST(work, delayed_flush) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, queued_cancel) {}

TEST(work, queued_cancel_sync) {}

TEST(work, delayed_cancel) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, delayed_cancel_sync) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, delayed_cancel_sync_wait) {
    GTEST_SKIP() << "Functionality not implemented yet\n";
}

TEST(work, running_cancel) {}

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
