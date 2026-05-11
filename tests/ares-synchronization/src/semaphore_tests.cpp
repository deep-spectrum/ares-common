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
#include <gtest/gtest.h>

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
static void sem_give_task(ares::semaphore<count> &sem) {
    EXPECT_NO_THROW(sem.give());
}

template <size_t count>
static void sem_thread2thread(ares::semaphore<count> &sem) {
    std::thread t = std::thread(sem_give_task<count>, std::ref(sem));

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
