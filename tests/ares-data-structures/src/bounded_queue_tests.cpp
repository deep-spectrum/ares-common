/**
 * @file bounded_queue_tests.cpp
 *
 * @brief
 *
 * @date 5/7/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/data-structures/queue.hpp>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <thread_utils.hpp>

using namespace std::chrono_literals;

TEST(queue_api, bounded_queue_no_overwrite_basic_functionality_single) {
    ares::bounded_queue<int> cut;

    int data[] = {
        0xAAAA,
        0xBBBB,
        0xCCCC,
    };
    int val;

    // Empty tests & size tests SINGLE
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.put(data[0]);
    ASSERT_FALSE(cut.empty());
    ASSERT_TRUE(cut.full());
    ASSERT_EQ(cut.size(), 1);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    // Empty & size multiple entries
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_THROW(cut.put_nonblocking(data[1]), ares::queue_exception);
    ASSERT_EQ(cut.size(), 1);

    cut.clear();

    // Basic putting and getting single
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);

    // Basic putting and getting multi
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_THROW(cut.put_nonblocking(data[1]), ares::queue_exception);
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);

    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_THROW(cut.put_nonblocking(data[2]), ares::queue_exception);
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[1]);

    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_THROW(cut.put_nonblocking(data[0]), ares::queue_exception);
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[2]);
}

TEST(queue_api, bounded_queue_no_overwrite_basic_functionality_multi) {
    ares::bounded_queue<int, 3> cut;

    int data[] = {
        0xAAAA,
        0xBBBB,
        0xCCCC,
    };
    int val;

    // Empty tests & size tests
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.put(data[0]);
    ASSERT_FALSE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 1);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    // Empty & size multiple entries
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_EQ(cut.size(), 2);

    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_EQ(cut.size(), 3);
    ASSERT_TRUE(cut.full());

    cut.clear();

    // Basic putting and getting single
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);

    // Basic putting and getting multi
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_THROW(cut.put_nonblocking(data[0]), ares::queue_exception);
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

TEST(queue_api, bounded_queue_overwrite_basic_functionality_single) {
    ares::bounded_queue<int, 1, true> cut;

    int data[] = {
        0xAAAA,
        0xBBBB,
        0xCCCC,
    };
    int val;

    // Empty tests & size tests SINGLE
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.put(data[0]);
    ASSERT_FALSE(cut.empty());
    ASSERT_TRUE(cut.full());
    ASSERT_EQ(cut.size(), 1);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    // Empty & size multiple entries
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_EQ(cut.size(), 1);

    cut.clear();

    // Basic putting and getting single
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);

    // Basic putting and getting multi
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[1]);

    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[2]);

    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);
}

TEST(queue_api, bounded_queue_overwrite_basic_functionality_multi) {
    ares::bounded_queue<int, 3, true> cut;

    int data[] = {
        0xAAAA,
        0xBBBB,
        0xCCCC,
    };
    int val;

    // Empty tests & size tests
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.put(data[0]);
    ASSERT_FALSE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 1);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    cut.clear();
    ASSERT_TRUE(cut.empty());
    ASSERT_FALSE(cut.full());
    ASSERT_EQ(cut.size(), 0);

    // Empty & size multiple entries
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_EQ(cut.size(), 2);

    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_EQ(cut.size(), 3);
    ASSERT_TRUE(cut.full());

    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 3);
    ASSERT_TRUE(cut.full());

    cut.clear();

    // Basic putting and getting single
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 1);
    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);

    // Basic putting and getting multi
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[1]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[2]));
    ASSERT_NO_THROW(cut.put_nonblocking(data[0]));
    ASSERT_EQ(cut.size(), 3);

    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 2);
    ASSERT_EQ(val, data[1]);

    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 1);
    ASSERT_EQ(val, data[2]);

    ASSERT_NO_THROW(val = cut.get_nonblocking());
    ASSERT_EQ(cut.size(), 0);
    ASSERT_EQ(val, data[0]);
}

template <size_t size, bool overwrite>
static void release_queue_wait(ares::bounded_queue<int, size, overwrite> &cut,
                               bool &released_var,
                               std::chrono::milliseconds timeout) {
    released_var = false;
    std::this_thread::sleep_for(timeout);
    released_var = true;
    cut.put(0);
}

TEST(queue_api, bounded_queue_empty_get) {
    ares::bounded_queue<int> single_no_overwrite_cut;
    ares::bounded_queue<int, 3> multi_no_overwrite_cut;
    ares::bounded_queue<int, 1, true> single_overwrite_cut;
    ares::bounded_queue<int, 3, true> multi_overwrite_cut;
    std::thread t[8];

    // single non-overwriting
    ASSERT_THROW(single_no_overwrite_cut.get(100ms), ares::queue_exception);
    ASSERT_THROW(single_no_overwrite_cut.get_nonblocking(),
                 ares::queue_exception);

    bool released = false;
    t[0] = std::thread(release_queue_wait<1, false>,
                       std::ref(single_no_overwrite_cut), std::ref(released),
                       500ms);
    ASSERT_NO_THROW(single_no_overwrite_cut.get()); // blocking call
    ASSERT_TRUE(released);

    released = false;
    t[1] = std::thread(release_queue_wait<1, false>,
                       std::ref(single_no_overwrite_cut), std::ref(released),
                       100ms);
    ASSERT_NO_THROW(single_no_overwrite_cut.get(1s)); // blocking 1 second
    ASSERT_TRUE(released);

    // multi non-overwriting
    ASSERT_THROW(multi_no_overwrite_cut.get(100ms), ares::queue_exception);
    ASSERT_THROW(multi_no_overwrite_cut.get_nonblocking(),
                 ares::queue_exception);

    released = false;
    t[2] = std::thread(release_queue_wait<3, false>,
                       std::ref(multi_no_overwrite_cut), std::ref(released),
                       500ms);
    ASSERT_NO_THROW(multi_no_overwrite_cut.get()); // blocking call
    ASSERT_TRUE(released);

    released = false;
    t[3] = std::thread(release_queue_wait<3, false>,
                       std::ref(multi_no_overwrite_cut), std::ref(released),
                       100ms);
    ASSERT_NO_THROW(multi_no_overwrite_cut.get(1s)); // blocking 1 second
    ASSERT_TRUE(released);

    // single overwriting
    ASSERT_THROW(single_overwrite_cut.get(100ms), ares::queue_exception);
    ASSERT_THROW(single_overwrite_cut.get_nonblocking(), ares::queue_exception);

    released = false;
    t[4] =
        std::thread(release_queue_wait<1, true>, std::ref(single_overwrite_cut),
                    std::ref(released), 500ms);
    ASSERT_NO_THROW(single_overwrite_cut.get()); // blocking call
    ASSERT_TRUE(released);

    released = false;
    t[5] =
        std::thread(release_queue_wait<1, true>, std::ref(single_overwrite_cut),
                    std::ref(released), 100ms);
    ASSERT_NO_THROW(single_overwrite_cut.get(1s)); // blocking 1 second
    ASSERT_TRUE(released);

    // multi overwriting
    ASSERT_THROW(multi_overwrite_cut.get(100ms), ares::queue_exception);
    ASSERT_THROW(multi_overwrite_cut.get_nonblocking(), ares::queue_exception);

    released = false;
    t[6] =
        std::thread(release_queue_wait<3, true>, std::ref(multi_overwrite_cut),
                    std::ref(released), 500ms);
    ASSERT_NO_THROW(multi_overwrite_cut.get()); // blocking call
    ASSERT_TRUE(released);

    released = false;
    t[7] =
        std::thread(release_queue_wait<3, true>, std::ref(multi_overwrite_cut),
                    std::ref(released), 100ms);
    ASSERT_NO_THROW(multi_overwrite_cut.get(1s)); // blocking 1 second
    ASSERT_TRUE(released);

    for (auto &thread : t) {
        thread.join();
    }
}
