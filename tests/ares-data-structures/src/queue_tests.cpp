/**
 * @file queue_tests.cpp
 *
 * @brief
 *
 * @date 5/6/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/data-structures/queue.hpp>
#include <gtest/gtest.h>
#include <thread>

TEST(queue, queue_basic_functionality) {
    ares::queue<int> cut;

    int data[] = {
        0xAAAA,
        0xBBBB,
        0xCCCC,
    };
    int val;

    // Empty tests & size tests
    ASSERT_TRUE(cut.empty());
    ASSERT_EQ(cut.size(), 0);

    cut.put(data[0]);
    ASSERT_FALSE(cut.empty());
    ASSERT_EQ(cut.size(), 1);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_EQ(cut.size(), 0);

    // Empty & size multiple entries
    cut.put(data[0]);
    cut.put(data[1]);
    ASSERT_FALSE(cut.empty());
    ASSERT_EQ(cut.size(), 2);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_EQ(cut.size(), 0);

    // Basic putting and getting
    // one element
    cut.put(data[0]);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);

    // multiple times
    cut.put(data[0]);
    cut.put(data[1]);
    cut.put(data[2]);
    ASSERT_EQ(cut.size(), 3);

    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 2);
    ASSERT_EQ(val, data[0]);

    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 1);
    ASSERT_EQ(val, data[1]);

    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[2]);
}
