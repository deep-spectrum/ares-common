/**
 * @file constants.hpp
 *
 * @brief
 *
 * @date 5/11/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_COMMON_SYNC_CONSTANTS_HPP
#define ARES_COMMON_SYNC_CONSTANTS_HPP

#if !defined(ARES_COMMON_SEMAPHORE_HPP)
#error                                                                         \
    "ares/synchronization/internal/constants.hpp should not be included directly"
#endif

#include <chrono>

namespace ares {
/**
 * Non-blocking timeout.
 */
constexpr auto no_wait = std::chrono::milliseconds::zero();

/**
 * No timeout.
 */
constexpr auto forever = std::chrono::milliseconds::max();
} // namespace ares

#endif // ARES_CONSTANTS_HPP
