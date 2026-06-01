/**
 * @file align_allocator.cpp
 *
 * @brief
 *
 * @date 5/27/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/allocators/align_allocator.hpp>
#include <ares/util.hpp>
#include <cassert>
#include <cstdlib>
#include <unistd.h>

namespace ares {
// Source - https://stackoverflow.com/a/12942652
// Posted by znkr, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-27, License - CC BY-SA 3.0

void *detail::allocate_aligned_memory(size_t align, size_t size) {
    assert(align >= sizeof(void *));
    assert(ares::power_of_2(align));

    if (size == 0) {
        return nullptr;
    }

    void *ptr = nullptr;
    int rc = posix_memalign(&ptr, align, size);

    if (rc != 0) {
        return nullptr;
    }

    return ptr;
}

void detail::deallocate_aligned_memory(void *ptr) noexcept { free(ptr); }
} // namespace ares
