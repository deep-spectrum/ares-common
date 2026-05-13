/**
 * @file task.hpp
 *
 * @brief Task implementation.
 *
 * @date 4/10/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_TASK_HPP
#define ARES_COMMON_TASK_HPP

#include <ares/synchronization/spinlock.hpp>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <string>
#include <thread>
#include <utility>

namespace ares {
/**
 * @class TaskException
 * @brief Exception class for task related errors.
 */
class TaskException : std::exception {
  public:
    /**
     * Constructor.
     * @param msg The exception message.
     */
    explicit TaskException(const char *msg) : _msg(msg) {}

    /**
     * The error message or reason.
     * @return The error message.
     */
    [[nodiscard]] const char *what() const noexcept override {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};

namespace internal {
template <typename Signature>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
    using ReturnType = R;
};

template <typename R, typename... Args>
struct FunctionTraits<R (*)(Args...)> {
    using ReturnType = R;
};
} // namespace internal

/**
 * @class Task
 * @tparam Signature The function signature for the task to execute.
 */
template <typename Signature>
class Task {
  public:
    using ReturnType = typename internal::FunctionTraits<Signature>::ReturnType;

    /**
     * Constructor.
     * @param[in] handler The task handler.
     * @throws TaskException if the handler is nullptr
     */
    explicit Task(std::function<Signature> handler) : _handler(handler) {
        if (handler == nullptr) {
            throw TaskException("Task handler cannot be `nullptr`");
        }
    }

    /**
     * Destructor.
     */
    ~Task();

    /**
     * Set the name of the task.
     *
     * @param[in] new_name The new name of the task.
     *
     * @return 0 if the name was set.
     * @return -EBUSY if the task is running.
     */
    int set_name(std::string new_name);

    /**
     * Retrieve the name of the task.
     * @return The name of the task.
     */
    [[nodiscard]] const char *get_name();

    /**
     * @brief Marks a task as essential or not.
     *
     * Marks a task as essential or non-essential. If a task is marked as
     * essential, then the system will abort if an uncaught exception is thrown.
     * Otherwise, the task will propagate the exception up.
     *
     * @param[in] essential_task Flag indicating if the task is essential for
     * the program.
     * @return 0 if the task was marked as essential.
     * @return -EBUSY if the task is running.
     */
    int set_essential(bool essential_task);

    /**
     * Check if the task is essential.
     * @return The essential status of the thread.
     */
    [[nodiscard]] bool get_essential();

    /**
     * @brief Start task execution.
     *
     * Creates a new thread and starts it running.
     *
     * @tparam Args Variable handler arguments.
     * @param args Task handler arguments.
     */
    template <typename... Args>
    void start(Args &&...args);

    /**
     * @brief Run task in calling thread.
     *
     * This will run the task in the calling thread.
     *
     * @tparam Args Variable handler arguments.
     * @param args Task handler arguments.
     */
    template <typename... Args>
    void run(Args &&...args);

    /**
     * @brief Sleep until the task exits.
     *
     * This will stall current thread execution until the timeout has expired or
     * the task has stopped execution.
     *
     * @param[in] timeout Upper bound time to wait for the thread to exit.
     * @return 0 success, target task thread has exited.
     * @return -ETIMEDOUT waiting period timed out.
     * @return -EALREADY task thread wasn't running.
     *
     * @note If timeout is set to std::chrono::milliseconds::max(), this will
     * stall indefinitely.
     */
    int
    join(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

    /**
     * Get task thread ID.
     * @return ID of the task thread.
     */
    [[nodiscard]] std::thread::id get_id() const;

    /**
     * Get the result of the task after running.
     * @return The return value of the handler, if any.
     *
     * @throws TaskException if the result cannot be determined.
     * @throws std::excetion Any exception the handler threw.
     */
    ReturnType get();

  private:
    enum states {
        READY,
        RUNNING,
        JOINED,
    };

    bool _essential = false;
    std::string _name;
    std::packaged_task<ReturnType()> _task;
    std::future<ReturnType> _future;
    std::thread _thread;
    std::thread::id _id;
    std::function<Signature> _handler;

    SpinLock _state_lock;
    states _state = READY;

    int _join();

    template <typename... Args>
    void _init_task(Args &&...args);
};

template <typename Signature>
Task<Signature>::~Task() {
    this->join();
}

template <typename Signature>
int Task<Signature>::set_name(std::string new_name) {
    std::unique_lock lock(_state_lock);
    if (_state == RUNNING) {
        return -EBUSY;
    }
    _name = std::move(new_name);
    return 0;
}

template <typename Signature>
const char *Task<Signature>::get_name() {
    std::unique_lock lock(_state_lock);
    return _name.c_str();
}

template <typename Signature>
int Task<Signature>::set_essential(bool essential_task) {
    std::unique_lock lock(_state_lock);
    if (_state == RUNNING) {
        return -EBUSY;
    }
    _essential = essential_task;
    return 0;
}

template <typename Signature>
bool Task<Signature>::get_essential() {
    std::unique_lock lock(_state_lock);
    return _essential;
}

template <typename Signature>
template <typename... Args>
void Task<Signature>::start(Args &&...args) {
    _init_task(std::forward<Args>(args)...);
    _thread = std::thread(std::move(_task));
    _id = _thread.get_id();
}

template <typename Signature>
template <typename... Args>
void Task<Signature>::run(Args &&...args) {
    _init_task(std::forward<Args>(args)...);
    _id = std::this_thread::get_id();
    _task();
    std::unique_lock lock(_state_lock);
    _state = JOINED;
}

template <typename Signature>
int Task<Signature>::join(const std::chrono::milliseconds timeout) {
    if (timeout == std::chrono::milliseconds::max()) {
        return _join();
    }

    auto status = _future.wait_for(timeout);
    if (status == std::future_status::ready) {
        return _join();
    }
    return -ETIMEDOUT;
}

template <typename Signature>
std::thread::id Task<Signature>::get_id() const {
    return _id;
}

template <typename Signature>
typename Task<Signature>::ReturnType Task<Signature>::get() {
    std::unique_lock lock(_state_lock);
    if (_state == READY) {
        throw TaskException(
            "Cannot get result from task that has not been started");
    }

    if (_state == RUNNING) {
        throw TaskException("Join task before getting the result");
    }
    _state = READY;
    return _future.get();
}

template <typename Signature>
int Task<Signature>::_join() {
    if (_thread.joinable()) {
        _thread.join();
        std::unique_lock lock(_state_lock);
        _state = JOINED;
        return 0;
    }
    return -EALREADY;
}

template <typename Signature>
template <typename... Args>
void Task<Signature>::_init_task(Args &&...args) {
    std::unique_lock lock(_state_lock);

    if (_state == RUNNING) {
        throw TaskException(
            "Cannot run/start a task that is already in the `run` state.");
    }

    _state = RUNNING;

    auto wrapper = [this, bound_args = std::make_tuple(std::forward<Args>(
                              args)...)]() -> decltype(auto) {
        try {
            return std::apply(
                [this](auto &&...unpacked_args) -> decltype(auto) {
                    return this->_handler(std::forward<decltype(unpacked_args)>(
                        unpacked_args)...);
                },
                bound_args);
        } catch ([[maybe_unused]] const std::exception &e) {
            if (_essential) {
                // todo: log
                std::abort();
            }
            throw;
        }
    };

    auto bound_task =
        std::bind(std::move(wrapper), std::forward<Args>(args)...);
    _task = std::packaged_task<ReturnType()>(std::move(bound_task));
    _future = _task.get_future();
}
} // namespace ares

#endif // ARES_COMMON_TASK_HPP
