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
