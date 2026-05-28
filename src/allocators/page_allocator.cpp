/**
 * @file page_allocator.cpp
 *
 * @brief
 *
 * @date 5/27/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/allocators/page_allocator.hpp>
#include <cstdlib>
#include <unistd.h>

const size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);

void *detail::allocate_aligned_page(size_t size) {
    if (size == 0) {
        return nullptr;
    }

    void *ptr = nullptr;
    int rc = posix_memalign(&ptr, PAGE_SIZE, size);

    if (rc != 0) {
        return nullptr;
    }

    return ptr;
}

void detail::deallocate_aligned_page(void *ptr) noexcept { free(ptr); }
