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
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

TEST(queue_api, queue_basic_functionality) {
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

static void release_queue_wait(ares::queue<int> &cut, bool &released_var,
                               std::chrono::milliseconds timeout) {
    released_var = false;
    std::this_thread::sleep_for(timeout);
    released_var = true;
    cut.put(0);
}

TEST(queue_api, queue_empty_get) {
    ares::queue<int> cut;
    bool released = false;

    // check if exception is thrown
    ASSERT_THROW(cut.get(100ms), ares::queue_exception);
    ASSERT_THROW(cut.get_nonblocking(), ares::queue_exception);

    // check if blocking get stalls indefinitely
    std::thread t1(release_queue_wait, std::ref(cut), std::ref(released),
                   500ms);

    cut.get();
    ASSERT_TRUE(released);

    // check if timeout get gets released after a while
    std::thread t2(release_queue_wait, std::ref(cut), std::ref(released),
                   100ms);

    ASSERT_NO_THROW(cut.get(1s));

    t1.join();
    t2.join();
}
