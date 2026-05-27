/**
 * @file page_allocator.hpp
 *
 * @brief
 *
 * @date 5/27/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_PAGE_ALLOCATOR_HPP
#define ARES_PAGE_ALLOCATOR_HPP

#include <memory>

extern const size_t PAGE_SIZE;

namespace detail {
void *allocate_aligned_page(size_t size);
void deallocate_aligned_page(void *ptr) noexcept;
} // namespace detail

template <typename T>
class PageAllocator;

template <>
class PageAllocator<void> {
  public:
    typedef void *pointer;
    typedef const void *const_pointer;
    typedef void value_type;

    template <class U>
    struct rebind {
        typedef PageAllocator<U> other;
    };
};

template <typename T>
class PageAllocator {
  public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef std::true_type propagate_on_container_move_assignment;

    template <class U>
    struct rebind {
        typedef PageAllocator<U> other;
    };

    PageAllocator() noexcept = default;

    template <class U>
    explicit PageAllocator(const PageAllocator<U> &) noexcept {}

    [[nodiscard]] static size_type max_size() noexcept {
        return (static_cast<size_type>(~0) - PAGE_SIZE) / sizeof(T);
    }

    pointer address(reference x) const noexcept { return std::addressof(x); }

    const_pointer address(const_reference x) const noexcept {
        return std::addressof(x);
    }

    static pointer allocate(size_type n,
                            PageAllocator<void>::const_pointer = nullptr) {
        [[maybe_unused]] const size_type alignment = PAGE_SIZE;
        void *ptr = detail::allocate_aligned_page(n * sizeof(T));
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }

        return static_cast<pointer>(ptr);
    }

    static void deallocate(pointer p, size_type) noexcept {
        return detail::deallocate_aligned_page(p);
    }

    template <class U, class... Args>
    void construct(U *p, Args &&...args) {
        ::new (reinterpret_cast<void *>(p)) U(std::forward<Args>(args)...);
    }

    static void destroy(pointer p) { p->~T(); }
};

template <typename T>
class PageAllocator<const T> {
  public:
    typedef T value_type;
    typedef const T *pointer;
    typedef const T *const_pointer;
    typedef const T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef std::true_type propagate_on_container_move_assignment;

    template <class U>
    struct rebind {
        typedef PageAllocator<U> other;
    };

    PageAllocator() noexcept = default;

    template <class U>
    explicit PageAllocator(const PageAllocator<U> &) noexcept {}

    [[nodiscard]] static size_type max_size() noexcept {
        return (static_cast<size_type>(~0) - PAGE_SIZE) / sizeof(T);
    }

    const_pointer address(const_reference x) const noexcept {
        return std::addressof(x);
    }

    static pointer allocate(size_type n,
                            PageAllocator<void>::const_pointer = nullptr) {
        [[maybe_unused]] const size_type alignment = PAGE_SIZE;
        void *ptr = detail::allocate_aligned_page(n * sizeof(T));
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }

        return static_cast<pointer>(ptr);
    }

    void deallocate(pointer p, size_type) noexcept {
        return detail::deallocate_aligned_page(p);
    }

    template <class U, class... Args>
    void construct(U *p, Args &&...args) {
        ::new (reinterpret_cast<void *>(p)) U(std::forward<Args>(args)...);
    }

    static void destroy(pointer p) { p->~T(); }
};

#endif // ARES_PAGE_ALLOCATOR_HPP
