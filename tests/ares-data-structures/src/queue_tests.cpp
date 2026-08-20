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
#include <helpers.hpp>
#include <thread>
#include <thread_utils.hpp>

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

static void thread_put(ares::queue<int> &cut, size_t num_items, bool &timed_out,
                       const std::chrono::milliseconds &max_execution_time) {
    int val = 0;
    timed_out = false;

    auto now = std::chrono::steady_clock::now;

    auto start = now();
    for (size_t i = 0u; i < num_items && !timed_out; i++) {
        cut.put(val);
        timed_out = (now() - start) > max_execution_time;
    }
}

static void thread_get(ares::queue<int> &cut, size_t &num_received,
                       const std::chrono::milliseconds &timeout) {
    num_received = 0;

    auto now = std::chrono::steady_clock::now;

    auto start = now();
    while ((now() - start) < timeout) {
        try {
            cut.get(1ms);
            num_received++;
        } catch (...) {
            // nop
        }
    }
}

TEST(queue_api, queue_thread2thread) {
    ares::queue<int> cut;
    size_t num_received;
    size_t expected_num_received = 10;
    bool timed_out;

    std::thread t1(thread_get, std::ref(cut), std::ref(num_received), 1s);
    std::thread t2(thread_put, std::ref(cut), expected_num_received,
                   std::ref(timed_out), 500ms);

    t1.join();
    t2.join();

    ASSERT_FALSE(timed_out);
    ASSERT_EQ(num_received, expected_num_received);
}

TEST(queue_api, queue_put2threads) {
    // spawn 2 threads, both putting while the parent gets
    ares::queue<int> cut;
    size_t num_to_send = 10;
    bool timed_out1 = false, timed_out2 = false;
    size_t num_received;

    std::thread t1(thread_put, std::ref(cut), num_to_send, std::ref(timed_out1),
                   500ms);
    std::thread t2(thread_put, std::ref(cut), num_to_send, std::ref(timed_out2),
                   500ms);

    thread_get(cut, num_received, 1s);

    t1.join();
    t2.join();

    ASSERT_FALSE(timed_out1);
    ASSERT_FALSE(timed_out2);
    ASSERT_EQ(num_received, (2 * num_to_send));
}

static void wait_for_queue(ares::queue<int> &cut, thread_prio prio, int &ret,
                           volatile bool &ready, volatile bool &error) {
    int prio_ret = change_thread_prio(prio);
    if (prio_ret < 0) {
        error = true;
        return;
    }
    ready = true;
    ret = cut.get();
}

TEST(queue_api, queue_multithread_competition) {
    // spawn 3 threads, 1 high prio, 1 med prio, 1 low prio. parent puts while
    // the 3 receive.

#if defined(SKIP_RT_TESTS)
    GTEST_SKIP() << "Realtime tests disabled";
#endif

    ares::queue<int> cut;
    int data[3] = {
        0xaaaa,
        0xbbbb,
        0xcccc,
    };
    int ret[3];
    volatile bool ready[3] = {false};
    volatile bool errors[3] = {false};

    std::thread t1(wait_for_queue, std::ref(cut), HIGH, std::ref(ret[0]),
                   std::ref(ready[0]), std::ref(errors[0]));
    std::thread t2(wait_for_queue, std::ref(cut), MED, std::ref(ret[1]),
                   std::ref(ready[1]), std::ref(errors[1]));
    std::thread t3(wait_for_queue, std::ref(cut), LOW, std::ref(ret[2]),
                   std::ref(ready[2]), std::ref(errors[2]));

    std::this_thread::sleep_for(100ms);

    bool error = false;
    for (size_t i = 0u; i < 3; i++) {
        while (!ready[i] && !errors[i])
            ;
        error = errors[i] || error;
    }

    for (int &i : data) {
        cut.put(i);
    }

    t1.join();
    t2.join();
    t3.join();

    if (error) {
        GTEST_SKIP() << "Unable to update thread priorities. Please check the "
                        "realtime thread priority configurations.";
    }

    for (size_t i = 0u; i < 3; i++) {
        ASSERT_EQ(data[i], ret[i]);
    }
}

static void thread_get_cb_no_timeout(BlockHelper &helper, ares::queue<int> &cut,
                                     size_t num_expected) {
    size_t num_received = 0;

    while (num_received < num_expected) {
        cut.get([&helper] { helper.increment_value(); });
        num_received++;
    }
}

TEST(queue_api, queue_blocking_callbacks) {
    ares::queue<int> cut;
    size_t num_to_send = 10;
    BlockHelper helper;
    std::chrono::milliseconds timeout = 100ms;

    std::thread t(thread_get_cb_no_timeout, std::ref(helper), std::ref(cut),
                  num_to_send);

    for (size_t i = 0; i < num_to_send; i++) {
        cut.put(0);
        std::this_thread::sleep_for(1ms);
    }

    t.join();

    EXPECT_NE(helper.value, 0);

    helper.value = 0;
    try {
        cut.get(timeout, [&helper] { helper.increment_value(); });
    } catch (...) {
        // nop
    }

    EXPECT_NE(helper.value, 0);
}
