/**
 * @file util.hpp
 *
 * @brief
 *
 * @date 4/2/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_UTIL_HPP
#define ARES_UTIL_HPP

/**
 * Get a pointer to a structure containing the element.
 *
 * Example:
 *
 * ```c
 * struct Foo {
 *     int bar;
 * }
 *
 * Foo my_foo;
 * int *ptr = &my_foo.bar;
 *
 * Foo *container = container_of(ptr, &Foo::bar);
 * ```
 *
 * @tparam Parent The parent type.
 * @tparam Member The data member type.
 * @param ptr Pointer to the data member.
 * @param member_ptr Pointer to the field the data member points to.
 * @return Pointer to the parent object.
 */
template <typename Parent, typename Member>
Parent *container_of(Member *ptr, Member Parent::*member_ptr) {
    size_t offset = reinterpret_cast<size_t>(
        &(static_cast<Parent *>(nullptr)->*member_ptr));
    return reinterpret_cast<Parent *>(reinterpret_cast<char *>(ptr) - offset);
}

#endif // ARES_UTIL_HPP
