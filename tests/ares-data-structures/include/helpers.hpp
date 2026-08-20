/**
 * @file helpers.hpp
 *
 * @brief
 *
 * @date 8/20/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_HELPERS_HPP
#define ARES_HELPERS_HPP

#include <stdexcept>

struct BlockHelper {
    uint64_t value = 0;

    void increment_value() {
        value++;

        if (value > static_cast<uint64_t>(INT64_MAX)) {
            throw std::runtime_error("Timed out");
        }
    }
};

#endif // ARES_HELPERS_HPP
