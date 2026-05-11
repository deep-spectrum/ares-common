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
