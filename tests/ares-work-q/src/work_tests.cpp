/**
 * @file work_tests.cpp
 *
 * @brief
 *
 * @date 5/14/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/work-q/work_q.hpp>
#include <gtest/gtest.h>

static void counter_handler(ares::Work *work) {
    // todo
}

TEST(work, unstarted) {
    int rc;
    ares::WorkQ not_started_q;

    ares::Work work(counter_handler);
    EXPECT_EQ(work.work_busy_get(), 0);

    rc = not_started_q.submit(&work);
    EXPECT_EQ(rc, -ENODEV);
}
