/**
 * @file semaphore.hpp
 *
 * @brief
 *
 * @date 4/13/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */
 

#ifndef ARES_SEMAPHORE_HPP
#define ARES_SEMAPHORE_HPP

#include <ares/data-structures/queue.hpp>
#include <chrono>
#include <exception>
#include <string>

namespace ares {
    class timeout_exception : public std::exception {
    public:
        explicit timeout_exception(const char *msg) : _msg(msg) {}

        [[nodiscard]] const char *what() const noexcept override {
            return _msg.c_str();
        }

    private:
        std::string _msg;
    };

    template <size_t count = 1>
    class semaphore {
    public:
        semaphore() = delete;
        explicit semaphore(size_t init_cnt = count);

        void lock();
        void lock(std::chrono::milliseconds timeout);
        void unlock();
        bool try_lock();

        void lock_all();

    private:
        bounded_queue<uint8_t, count, true> _sem;
    };

    template<size_t count>
    semaphore<count>::semaphore(size_t init_cnt) {
        init_cnt = std::max(init_cnt, count);

        for (size_t i = 0; i < init_cnt; i++) {
            _sem.put_nonblocking(0);
        }
    }

    template<size_t count>
    void semaphore<count>::lock() {
        _sem.get();
    }

    template<size_t count>
    void semaphore<count>::lock(std::chrono::milliseconds timeout) {
        try {
            _sem.get(timeout);
        } catch (const queue_exception &exc) {
            if (exc.reason() == queue_exception::QUEUE_TIMEOUT) {
                throw timeout_exception("Semaphore `take` timed out");
            }
            throw;
        }
    }

    template<size_t count>
    void semaphore<count>::unlock() {
        _sem.put_nonblocking(0);
    }

    template<size_t count>
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

    template<size_t count>
    void semaphore<count>::lock_all() {
        _sem.clear();
    }
}

#endif //ARES_SEMAPHORE_HPP
