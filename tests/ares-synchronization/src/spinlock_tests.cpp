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
#include <chrono>
#include <gtest/gtest.h>

using namespace std::chrono_literals;

TEST(spinlock_api, test_spinlock_basic) {
    ares::SpinLock l;

    ASSERT_FALSE(l.locked());

    l.lock();

    ASSERT_TRUE(l.locked());

    l.unlock();

    ASSERT_FALSE(l.locked());
}

struct BounceLockParams {
    int id;
    bool try_lock;
};

struct BounceLockResults {
    volatile int trylock_failures = 0;
    volatile int trylock_successes = 0;
};

struct BounceLockState {
    volatile int owner = 0;
    volatile bool done = false;
};

static void busy_wait(const std::chrono::microseconds &timeout) {
    auto now = std::chrono::steady_clock::now;
    auto start = now();
    while ((now() - start) < timeout)
        ;
}

static void bounce_once(ares::SpinLock &lock, BounceLockParams params,
                        BounceLockResults &results, BounceLockState &state) {
    bool locked = false;

    for (size_t i = 0; i < 10000; i++) {
        if (params.try_lock) {
            bool ret = lock.try_lock();
            if (!ret) {
                results.trylock_failures = results.trylock_failures + 1;
                continue;
            }
            results.trylock_successes = results.trylock_successes + 1;
        } else {
            lock.lock();
        }

        if (state.owner != params.id) {
            locked = true;
            break;
        }

        lock.unlock();
        busy_wait(1us);
    }

    if (!locked && state.done) {
        return;
    }

    EXPECT_TRUE(locked) << "Other CPU did not get lock in 10000 tries";

    state.owner = params.id;

    for (size_t i = 0; i < 5; i++) {
        EXPECT_EQ(state.owner, params.id);
        busy_wait(5us);
    }

    lock.unlock();
}

static void cpu1_fn(ares::SpinLock &cut, BounceLockResults &results,
                    BounceLockState &state) {
    BounceLockParams params = {
        .id = 4321,
        .try_lock = false,
    };

    while (!state.done) {
        bounce_once(cut, params, results, state);

        if (::testing::Test::HasFailure()) {
            break;
        }
    }
}

TEST(spinlock_api, test_spinlock_bounce) {
    ares::SpinLock cut;

    BounceLockParams params = {
        .id = 1234,
        .try_lock = false,
    };

    BounceLockResults results;
    BounceLockState state;

    std::thread t(cpu1_fn, std::ref(cut), std::ref(results), std::ref(state));
    busy_wait(10us);

    for (size_t i = 0; i < 10000; i++) {
        bounce_once(cut, params, results, state);

        if (HasFailure()) {
            break;
        }
    }

    state.done = true;
    t.join();
}
