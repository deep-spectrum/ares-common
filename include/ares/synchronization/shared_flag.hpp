/**
 * @file shared_flag.hpp
 *
 * @brief
 *
 * @date 8/18/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_SHARED_FLAG_HPP
#define ARES_SHARED_FLAG_HPP

#include <mutex>
#include <shared_mutex>

namespace ares {
/**
 * @class SharedFlag
 * A thread-safe condition flag. Provides shared access for multiple concurrent
 * readers, but enforces exclusive access for a single writer thread during
 * updates.
 */
class SharedFlag {
  public:
    /**
     * Default constructor.
     */
    SharedFlag() = default;

    /**
     * Constructor.
     * @param init_condition The initial flag value.
     */
    explicit SharedFlag(bool init_condition) : _flag(init_condition) {}

    /**
     * Read the current flag value.
     * @return The current flag value.
     */
    bool read() const {
        std::shared_lock lock(_lock);
        return _flag;
    }

    /**
     * Update the current flag value.
     * @param condition The new condition.
     */
    void write(bool condition) {
        std::unique_lock lock(_lock);
        _flag = condition;
    }

    /**
     * Bool operator.
     */
    explicit operator bool() const { return read(); }

    /**
     * Set the new condition.
     * @param condition The new condition.
     * @return Reference to object.
     */
    SharedFlag &operator=(bool condition) {
        write(condition);
        return *this;
    }

  private:
    mutable std::shared_mutex _lock;
    bool _flag = false;
};
} // namespace ares

#endif // ARES_SHARED_FLAG_HPP
