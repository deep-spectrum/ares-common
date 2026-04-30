/**
 * @file spinlock.hpp
 *
 * @brief Spin lock implementation.
 *
 * @date 4/9/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_SPINLOCK_HPP
#define ARES_SPINLOCK_HPP

#include <atomic>

namespace ares {
/**
 * @class SpinLock
 */
class SpinLock {
    std::atomic_flag _locked = ATOMIC_FLAG_INIT;

  public:
    /**
     * Lock the spinlock.
     */
    void lock() {
        while (_locked.test_and_set(std::memory_order_acquire))
            ;
    }

    /**
     * Attempt to lock the spinlock.
     * @return `true` if the spinlock was locked. `false` otherwise.
     */
    bool try_lock() { return _locked.test_and_set(std::memory_order_acquire); }

    /**
     * Unlock the spinlock.
     */
    void unlock() { _locked.clear(std::memory_order_release); }
};
} // namespace ares

#endif // ARES_SPINLOCK_HPP
