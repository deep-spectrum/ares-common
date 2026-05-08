/**
 * @file spinlock_tests.cpp
 *
 * @brief
 *
 * @date 5/8/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/synchronization/spinlock.hpp>
#include <gtest/gtest.h>

TEST(spinlock_api, test_spinlock_basic) {
    ares::SpinLock l;

    ASSERT_FALSE(l.locked());

    l.lock();

    ASSERT_TRUE(l.locked());

    l.unlock();

    ASSERT_FALSE(l.locked());
}
