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

struct ThreadPutParams {
    size_t num_items;
    std::chrono::milliseconds max_exec_time;
    std::chrono::milliseconds sleep_period;
};

struct ThreadPutResults {
    bool timed_out;
};

struct ThreadGetParams {
    std::chrono::milliseconds timeout;
    std::chrono::milliseconds sleep_period;
};

struct ThreadGetResults {
    size_t num_received;
    size_t num_skipped;
};

template <size_t size, bool overwrite>
static void thread_put(ares::bounded_queue<int, size, overwrite> &cut,
                       ThreadPutParams params, ThreadPutResults &results) {
    int val = 0;
    results.timed_out = false;

    auto now = std::chrono::steady_clock::now;

    auto start = now();
    for (size_t i = 0u; i < params.num_items && !results.timed_out;
         i++, val++) {
        try {
            cut.put(val, params.max_exec_time);
        } catch (const ares::queue_exception &) {
            // nop
        }
        results.timed_out = (now() - start) > params.max_exec_time;

        if (params.sleep_period != 0ms) {
            std::this_thread::sleep_for(params.sleep_period);
        }
    }
}

template <size_t size, bool overwrite>
static void thread_get(ares::bounded_queue<int, size, overwrite> &cut,
                       ThreadGetParams params, ThreadGetResults &results) {
    results.num_received = 0;

    auto now = std::chrono::steady_clock::now;

    auto start = now();
    while ((now() - start) < params.timeout) {
        try {
            cut.get(params.timeout);
            results.num_received++;
        } catch (...) {
            // nop
        }

        if (params.sleep_period != 0ms) {
            std::this_thread::sleep_for(params.sleep_period);
        }
    }
}

template <size_t size, bool overwrite>
static void
run_thread2thread_test(ares::bounded_queue<int, size, overwrite> &cut,
                       ThreadPutParams put_params, ThreadGetParams get_params) {
    ThreadPutResults put_results{};
    ThreadGetResults get_results{};
    std::thread threads[2];

    threads[0] = std::thread(thread_put<size, overwrite>, std::ref(cut),
                             put_params, std::ref(put_results));
    threads[1] = std::thread(thread_get<size, overwrite>, std::ref(cut),
                             get_params, std::ref(get_results));

    for (auto &t : threads) {
        t.join();
    }

    ASSERT_FALSE(put_results.timed_out);

    if (!overwrite) {
        ASSERT_EQ(put_params.num_items, get_results.num_received);
    } else {
        // todo
    }
}

TEST(queue_api, bounded_queue_single_blocking_thread2thread) {
    ares::bounded_queue<int> cut;

    ThreadPutParams put_params = {
        .num_items = 100,
        .max_exec_time = 1s,
        .sleep_period = 0ms,
    };

    ThreadGetParams get_params = {
        .timeout = 1s,
        .sleep_period = 0ms,
    };

    // basic thread2thread
    run_thread2thread_test(cut, put_params, get_params);

    // fast putting, slow getting
    put_params.max_exec_time = 5s;
    get_params.timeout = 10s;
    get_params.sleep_period = 50ms;
    run_thread2thread_test(cut, put_params, get_params);

    // slow putting, fast getting
    put_params.max_exec_time = 100s;
    put_params.sleep_period = 100ms;
    get_params.timeout = 10s;
    get_params.sleep_period = 0ms;
    run_thread2thread_test(cut, put_params, get_params);
}

TEST(queue_api, bounded_queue_multi_blocking_thread2thread) {
    ares::bounded_queue<int, 3> cut;

    ThreadPutParams put_params = {
        .num_items = 100,
        .max_exec_time = 1s,
        .sleep_period = 0ms,
    };

    ThreadGetParams get_params = {
        .timeout = 1s,
        .sleep_period = 0ms,
    };

    // basic thread2thread
    run_thread2thread_test(cut, put_params, get_params);

    // fast putting, slow getting
    put_params.max_exec_time = 5s;
    get_params.timeout = 10s;
    get_params.sleep_period = 50ms;
    run_thread2thread_test(cut, put_params, get_params);

    // slow putting, fast getting
    put_params.max_exec_time = 100s;
    put_params.sleep_period = 100ms;
    get_params.timeout = 10s;
    get_params.sleep_period = 0ms;
    run_thread2thread_test(cut, put_params, get_params);
}
