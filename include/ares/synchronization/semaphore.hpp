/**
 * @file semaphore.hpp
 *
 * @brief Semaphore implementation.
 *
 * @date 4/13/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_SEMAPHORE_HPP
#define ARES_COMMON_SEMAPHORE_HPP

#include <ares/data-structures/queue.hpp>
#include <chrono>
#include <exception>
#include <string>

namespace ares {
/**
 * @class timeout_exception
 * Timeout exception class.
 */
class timeout_exception : public std::exception {
  public:
    /**
     * Constructor.
     * @param msg The exception message.
     */
    explicit timeout_exception(const char *msg) : _msg(msg) {}

    /**
     * Retrieve the exception message.
     * @return The exception message.
     */
    [[nodiscard]] const char *what() const noexcept override {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};

/**
 * @class semaphore
 * @tparam count Maximum permitted semaphore count.
 *
 * Allows threads to synchronize their actions.
 */
template <size_t count = 1>
class semaphore {
    static_assert(count > 0, "Semaphore must have a positive, non-zero count");

  public:
    /**
     * Default constructor.
     */
    semaphore();

    /**
     * Constructor that specifies an initial count.
     * @param[in] init_cnt Initial semaphore count.
     *
     * @note If the count is `0`, then the semaphore is locked upon creation.
     */
    explicit semaphore(size_t init_cnt);

    /**
     * Take the semaphore. Blocks indefinitely if it is not available.
     */
    void lock();

    /**
     * Take the semaphore. Blocks for the given amount of time.
     *
     * @param[in] timeout The maximum time the semaphore is allowed to block.
     *
     * @note If timeout is std::chrono::milliseconds::zero(), then the operation
     * becomes non-blocking.
     * @note If timeout is std::chrono::milliseconds::max(), then the operation
     * is the same as semaphore::lock().
     *
     * @throws timeout_exception if waiting period timeout out.
     */
    void lock(std::chrono::milliseconds &timeout);

    /**
     * Same as semaphore::lock().
     */
    void take();

    /**
     * Same as semaphore::lock().
     *
     * @param[in] timeout The maximum time the semaphore is allowed to block.
     *
     * @note If timeout is std::chrono::milliseconds::zero(), then the operation
     * becomes non-blocking.
     * @note If timeout is std::chrono::milliseconds::max(), then the operation
     * is the same as semaphore::take().
     */
    void take(std::chrono::milliseconds &timeout);

    /**
     * Give the semaphore.
     */
    void unlock();

    /**
     * Same as semaphore::unlock().
     */
    void give();

    /**
     * Tries to take the semaphore.
     * @return `true` if the semaphore was taken. `false` if the semaphore could
     * not be taken.
     */
    bool try_lock();

  private:
    bounded_queue<uint8_t, count, true> _sem;
};

template <size_t count>
semaphore<count>::semaphore() {
    for (size_t i = 0; i < count; i++) {
        _sem.put_nonblocking(0);
    }
}

template <size_t count>
semaphore<count>::semaphore(size_t init_cnt) {
    init_cnt = std::min(init_cnt, count);

    for (size_t i = 0; i < init_cnt; i++) {
        _sem.put_nonblocking(0);
    }
}

template <size_t count>
void semaphore<count>::lock() {
    _sem.get();
}

template <size_t count>
void semaphore<count>::take() {
    lock();
}

template <size_t count>
void semaphore<count>::take(std::chrono::milliseconds &timeout) {
    lock(timeout);
}

template <size_t count>
void semaphore<count>::lock(std::chrono::milliseconds &timeout) {
    try {
        _sem.get(timeout);
    } catch (const queue_exception &exc) {
        if (exc.reason() == queue_exception::QUEUE_TIMEOUT) {
            throw timeout_exception("Semaphore `take` timed out");
        }
        throw;
    }
}

template <size_t count>
void semaphore<count>::unlock() {
    _sem.put_nonblocking(0);
}

template <size_t count>
void semaphore<count>::give() {
    unlock();
}

template <size_t count>
bool semaphore<count>::try_lock() {
    bool ret = true;
    try {
        _sem.get_nonblocking();
    } catch (const queue_exception &exc) {
        if (exc.reason() != queue_exception::QUEUE_EMPTY) {
            throw;
        }
        ret = false;
    }

    return ret;
}
} // namespace ares

#endif // ARES_COMMON_SEMAPHORE_HPP
