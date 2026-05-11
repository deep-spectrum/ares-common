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

template <size_t cnt1, size_t cnt2>
static void sem_multiple_helper(ares::semaphore<cnt1> &multi_sem,
                                ares::semaphore<cnt2> &simple_sem) {
    EXPECT_NO_THROW(multi_sem.take());
    simple_sem.give();
}

template <size_t cnt1, size_t cnt2>
static std::thread
sem_multiple_thread_helper(ares::semaphore<cnt1> &multi_sem,
                           ares::semaphore<cnt2> &simple_sem) {
    return std::thread(sem_multiple_helper<cnt1, cnt2>, std::ref(multi_sem),
                       std::ref(simple_sem));
}

TEST(semaphore_api, sem_multiple_threads_wait) {
    constexpr size_t threads_waiting = 5;

    ares::semaphore<10> multi_sem(0);
    ares::semaphore<10> simple_sem(0);

    std::thread threads[threads_waiting];

    for (size_t repeat_cnt = 0; repeat_cnt < 2; repeat_cnt++) {
        for (auto &t : threads) {
            t = sem_multiple_thread_helper(multi_sem, simple_sem);
        }

        std::this_thread::sleep_for(500ms);

        for (size_t i = 0; i < threads_waiting; i++) {
            multi_sem.give();
        }

        std::this_thread::sleep_for(500ms);

        for (size_t i = 0; i < threads_waiting; i++) {
            EXPECT_NO_THROW(simple_sem.take());
        }

        EXPECT_EQ(simple_sem.get_count(), 0);
        EXPECT_EQ(multi_sem.get_count(), 0);

        for (auto &t : threads) {
            t.join();
        }

        if (HasFailure()) {
            break;
        }
    }
}

enum operation { ADD, SUB };

template <size_t cnt>
static void mutual_exclusion_test_helper(ares::semaphore<cnt> &sem,
                                         int &critical_var, operation op) {
    for (size_t i = 0; i < 1000; i++) {
        EXPECT_NO_THROW(sem.take());

        int tmp = critical_var;

        if (op == ADD) {
            critical_var += 1;
            EXPECT_EQ(critical_var, (tmp + 1));
        } else if (op == SUB) {
            critical_var -= 1;
            EXPECT_EQ(critical_var, (tmp - 1));
        }

        sem.give();
    }
}

template <size_t cnt>
static std::thread mutual_exclusion_thread_helper(ares::semaphore<cnt> &sem,
                                                  int &critical_var,
                                                  operation op) {
    return std::thread(mutual_exclusion_test_helper<cnt>, std::ref(sem),
                       std::ref(critical_var), op);
}

TEST(semaphore_api, mutual_exclusion) {
    ares::semaphore<10> sem(0);

    int crit_val = 0;

    std::thread t1 = mutual_exclusion_thread_helper(sem, crit_val, ADD);
    std::thread t2 = mutual_exclusion_thread_helper(sem, crit_val, SUB);

    std::this_thread::sleep_for(100ms);

    sem.give();
    t1.join();
    t2.join();

    ASSERT_EQ(crit_val, 0);
}
