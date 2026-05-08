/**
 * @file spinlock.hpp
 *
 * @brief Spin lock implementation.
 *
 * @date 4/9/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_SPINLOCK_HPP
#define ARES_COMMON_SPINLOCK_HPP

#include <atomic>

namespace ares {
/**
 * @class SpinLock
 * Spin lock implementation. This will make the thread busy wait until the lock
 * is available.
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

#if __cplusplus >= 202002L
    [[nodiscard]] bool locked() const {
        return _locked.test(std::memory_order_acquire);
    }
#endif
};
} // namespace ares

#endif // ARES_COMMON_SPINLOCK_HPP
