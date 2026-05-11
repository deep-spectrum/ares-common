/**
 * @file semaphore_tests.cpp
 *
 * @brief
 *
 * @date 5/8/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/synchronization/semaphore.hpp>
#include <chrono>
#include <gtest/gtest.h>
#include <thread_utils.hpp>

using namespace std::chrono_literals;

TEST(semaphore_api, basic_operation) {
    ares::semaphore single_sem;
    ares::semaphore<2> multi_sem;

    ASSERT_EQ(single_sem.get_count(), 1);
    ASSERT_EQ(multi_sem.get_count(), 2);

    ASSERT_NO_THROW(single_sem.take(ares::no_wait));
    ASSERT_NO_THROW(multi_sem.take(ares::no_wait));

    ASSERT_EQ(single_sem.get_count(), 0);
    ASSERT_EQ(multi_sem.get_count(), 1);

    ASSERT_NO_THROW(multi_sem.take(ares::no_wait));
    ASSERT_EQ(multi_sem.get_count(), 0);

    ASSERT_NO_THROW(single_sem.give());
    ASSERT_EQ(single_sem.get_count(), 1);

    ASSERT_NO_THROW(multi_sem.give());
    ASSERT_EQ(multi_sem.get_count(), 1);

    ASSERT_NO_THROW(multi_sem.give());
    ASSERT_EQ(multi_sem.get_count(), 2);
}

template <size_t count>
static void sem_give_task(ares::semaphore<count> &sem,
                          const std::chrono::milliseconds sleep_time,
                          bool &given) {
    if (sleep_time != std::chrono::milliseconds::zero()) {
        std::this_thread::sleep_for(sleep_time);
    }
    given = true;
    EXPECT_NO_THROW(sem.give());
}

template <size_t count>
static void sem_thread2thread(ares::semaphore<count> &sem) {
    bool placeholder = false;
    std::thread t = std::thread(sem_give_task<count>, std::ref(sem), 0s,
                                std::ref(placeholder));

    EXPECT_NO_THROW(sem.take());

    t.join();
}

TEST(semaphore_api, thread2thread) {
    ares::semaphore single_sem(0);
    ares::semaphore<10> multi_sem(0);

    sem_thread2thread(single_sem);
    sem_thread2thread(multi_sem);
}

TEST(semaphore_api, sem_init) {
    // default constructor already tested in basic operation...
    ares::semaphore single_sem(0);
    ares::semaphore<10> multi_sem(0);

    ASSERT_EQ(single_sem.get_count(), 0);
    ASSERT_EQ(multi_sem.get_count(), 0);

    // invalid values
    ares::semaphore single_sem_invalid(500);
    ASSERT_EQ(single_sem_invalid.get_count(), 1);

    ares::semaphore<10> multi_sem_invalid(500);
    ASSERT_EQ(multi_sem_invalid.get_count(), 10);
}

TEST(semaphore_api, sem_take_nowait) {
    ares::semaphore<10> sem;

    for (ssize_t i = 9; i >= 0; i--) {
        ASSERT_NO_THROW(sem.take(ares::no_wait));
        ASSERT_EQ(sem.get_count(), i);
    }
}

TEST(semaphore_api, sem_take_nowait_fails) {
    ares::semaphore<10> sem(0);

    for (ssize_t i = 9; i >= 0; i--) {
        ASSERT_THROW(sem.take(ares::no_wait), ares::semaphore_exception);
    }
}

TEST(semaphore_api, sem_take_timeout) {
    ares::semaphore<10> sem;

    for (ssize_t i = 9; i >= 0; i--) {
        ASSERT_NO_THROW(sem.take(100ms));
        ASSERT_EQ(sem.get_count(), i);
    }
}

TEST(semaphore_api, sem_take_timeout_fails) {
    ares::semaphore<10> sem(0);

    for (ssize_t i = 9; i >= 0; i--) {
        ASSERT_THROW(sem.take(100ms), ares::timeout_exception);
    }
}

TEST(semaphore_api, sem_take_forever) {
    ares::semaphore<10> sem(0);
    bool given = false;

    std::thread t =
        std::thread(sem_give_task<10>, std::ref(sem), 1s, std::ref(given));

    ASSERT_EQ(sem.get_count(), 0);
    ASSERT_NO_THROW(sem.take());
    ASSERT_TRUE(given);
    t.join();
}

struct ThreadSemParams {
    thread_prio prio;
};

struct ThreadSemResults {
    volatile bool error = false;
    volatile bool initialized = false;
};

template <size_t sem1_count, size_t sem2_count>
static void sem_take_multiple_helper(ares::semaphore<sem1_count> &prio_sem,
                                     ares::semaphore<sem2_count> &multi_sem,
                                     ThreadSemParams params,
                                     ThreadSemResults &results) {

    if (change_thread_prio(params.prio) != 0) {
        results.error = true;
        return;
    }

    results.initialized = true;
    EXPECT_NO_THROW(prio_sem.take());
    EXPECT_NO_THROW(multi_sem.take());
    EXPECT_NO_THROW(prio_sem.give());
}

template <size_t sem1_count, size_t sem2_count>
static std::thread create_thread_helper(ares::semaphore<sem1_count> &prio_sem,
                                        ares::semaphore<sem2_count> &multi_sem,
                                        ThreadSemParams params,
                                        ThreadSemResults &results) {
    return std::thread(sem_take_multiple_helper<sem1_count, sem2_count>,
                       std::ref(prio_sem), std::ref(multi_sem), params,
                       std::ref(results));
}

TEST(semaphore_api, sem_take_multiple) {
    constexpr size_t high_prio_long = 2;
    constexpr size_t high_prio = 3;
    constexpr size_t med_prio = 1;
    constexpr size_t low_prio = 0;
    ares::semaphore<10> multi_sem(0);
    ares::semaphore<> prio_sems[4] = {ares::semaphore(0), ares::semaphore(0),
                                      ares::semaphore(0), ares::semaphore(0)};

    for (size_t i = 0; i < 4; i++) {
        ASSERT_EQ(prio_sems[i].get_count(), 0) << "Semaphore " << i;
    }

    std::thread threads[4];
    ThreadSemParams params[4] = {
        {
            LOW,
        },
        {
            MED,
        },
        {
            HIGH,
        },

        {
            HIGH,
        },
    };
    ThreadSemResults results[4];

    for (size_t i = 0; i < 4; i++) {
        threads[i] = create_thread_helper(prio_sems[i], multi_sem, params[i],
                                          results[i]);
    }

    bool error = false;
    for (auto &i : results) {
        while (!i.initialized && !i.error)
            ;
        error = i.error || error;
    }

    std::this_thread::sleep_for(20ms);

    if (error) {
        GTEST_SKIP() << "Unable to update thread priorities";
    }

    prio_sems[high_prio_long].give();
    prio_sems[med_prio].give();
    prio_sems[low_prio].give();

    std::this_thread::sleep_for(100ms);

    prio_sems[high_prio].give();
    std::this_thread::sleep_for(20ms);

    multi_sem.give();
    std::this_thread::sleep_for(200ms);

    EXPECT_EQ(prio_sems[high_prio_long].get_count(), 1);

    EXPECT_EQ(prio_sems[high_prio].get_count(), 0);
    EXPECT_EQ(prio_sems[med_prio].get_count(), 0);
    EXPECT_EQ(prio_sems[low_prio].get_count(), 0);

    if (HasFailure()) {
        goto cleanup;
    }

    multi_sem.give();
    std::this_thread::sleep_for(200ms);

    EXPECT_EQ(prio_sems[high_prio_long].get_count(), 1);
    EXPECT_EQ(prio_sems[high_prio].get_count(), 1);

    EXPECT_EQ(prio_sems[med_prio].get_count(), 0);
    EXPECT_EQ(prio_sems[low_prio].get_count(), 0);

    if (HasFailure()) {
        goto cleanup;
    }

    multi_sem.give();
    std::this_thread::sleep_for(200ms);

    EXPECT_EQ(prio_sems[high_prio_long].get_count(), 1);
    EXPECT_EQ(prio_sems[high_prio].get_count(), 1);
    EXPECT_EQ(prio_sems[med_prio].get_count(), 1);

    EXPECT_EQ(prio_sems[low_prio].get_count(), 0);

    if (HasFailure()) {
        goto cleanup;
    }

    multi_sem.give();
    std::this_thread::sleep_for(200ms);

    EXPECT_EQ(prio_sems[high_prio_long].get_count(), 1);
    EXPECT_EQ(prio_sems[high_prio].get_count(), 1);
    EXPECT_EQ(prio_sems[med_prio].get_count(), 1);
    EXPECT_EQ(prio_sems[low_prio].get_count(), 1);

cleanup:
    for (size_t i = 0; i < 4; i++) {
        multi_sem.give();
    }

    for (auto &t : threads) {
        t.join();
    }
}

TEST(semaphore_api, sem_correct_count_limit) {
    ares::semaphore<10> sem(0);

    for (size_t i = 1; i <= 10; i++) {
        ASSERT_NO_THROW(sem.give());
        ASSERT_EQ(sem.get_count(), i);
    }

    for (size_t i = 0; i < 5; i++) {
        ASSERT_THROW(sem.give(), ares::semaphore_exception);
        ASSERT_EQ(sem.get_count(), 10);
    }

    for (ssize_t i = 9; i >= 0; i--) {
        ASSERT_NO_THROW(sem.take(ares::no_wait));
        ASSERT_EQ(sem.get_count(), i);
    }

    for (size_t i = 0; i < 5; i++) {
        ASSERT_THROW(sem.take(ares::no_wait), ares::semaphore_exception);
        ASSERT_EQ(sem.get_count(), 0);
    }
}
