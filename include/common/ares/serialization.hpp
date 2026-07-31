/**
 * @file serialization.hpp
 *
 * @brief
 *
 * @date 7/31/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_SERIALIZATION_HPP
#define ARES_SERIALIZATION_HPP

#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <vector>

namespace ares {

template <typename T>
void set_bit(T &bitfield, size_t bit, bool value) {
    constexpr size_t T_size = sizeof(T) * __CHAR_BIT__;
    assert(bit <= T_size);

    T mask = static_cast<T>(1) << bit;
    bitfield &= ~mask;

    if (value) {
        bitfield |= mask;
    }
}

template <typename T>
bool check_bit(const T bitfield, size_t bit) {
    constexpr size_t T_size = sizeof(T) * __CHAR_BIT__;
    assert(bit <= T_size);

    T mask = static_cast<T>(1) << bit;
    return (bitfield & mask) != static_cast<T>(0);
}

template <typename T1, typename T2>
void set_bitfield(T1 &bitfield, size_t lsb, size_t num_bits, T2 value) {
    constexpr size_t T1_size = sizeof(T1) * __CHAR_BIT__;
    constexpr size_t T2_size = sizeof(T2) * __CHAR_BIT__;
    static_assert(sizeof(T1) >= sizeof(T2));
    assert(lsb < T1_size);
    assert(num_bits != 0);
    assert(num_bits <= T2_size);
    assert((lsb + num_bits) <= T1_size);

    T1 mask = (static_cast<T1>(1) << num_bits) - 1;
    T1 value_ = (static_cast<T1>(value) & mask) << lsb;

    bitfield &= ~(mask << lsb);
    bitfield |= value_;
}

template <typename T1, typename T2>
T2 get_bitfield(T1 bitfield, size_t lsb, size_t num_bits, T2 &value) {
    constexpr size_t T1_size = sizeof(T1) * __CHAR_BIT__;
    constexpr size_t T2_size = sizeof(T2) * __CHAR_BIT__;
    static_assert(sizeof(T1) >= sizeof(T2));
    assert(lsb < T1_size);
    assert(num_bits != 0);
    assert(num_bits <= T2_size);
    assert((lsb + num_bits) <= T1_size);

    T1 mask = (static_cast<T1>(1) << num_bits) - 1;
    T1 value_ = (bitfield >> lsb) & mask;
    value = static_cast<T2>(value_);
    return value;
}

template <size_t start_bit = 0, typename T, typename... Args>
void set_flags(T &bitfield, Args... flags) {
    constexpr size_t T_size = sizeof(T) * __CHAR_BIT__;
    static_assert(start_bit < T_size);
    static_assert(sizeof...(Args) <= T_size);
    static_assert(std::conjunction_v<std::is_same<Args, bool>...>);

    size_t bit = start_bit;

    auto process = [&](auto &&flag) {
        set_bit(bitfield, bit, flag);
        ++bit;
    };

    (process(std::forward<Args>(flags)), ...);
}

template <size_t start_bit = 0, typename T, typename... Args>
void get_flags(const T bitfield, Args &&...flags) {
    constexpr size_t T_size = sizeof(T) * __CHAR_BIT__;
    static_assert(start_bit < T_size);
    static_assert(sizeof...(Args) <= T_size);
    static_assert(std::conjunction_v<std::is_same<Args, bool &>...>);

    size_t bit = start_bit;

    auto process = [&](auto &&flag) {
        flag = check_bit(bitfield, bit);
        ++bit;
    };

    (process(std::forward<Args>(flags)), ...);
}

template <typename T, size_t buf_size>
struct DeserializeBuffer {
    T buffer[buf_size]{};
};

template <typename T>
struct is_deserialize_buffer : std::false_type {};

template <typename T, size_t buf_size>
struct is_deserialize_buffer<DeserializeBuffer<T, buf_size>> : std::true_type {
};

template <typename T>
inline constexpr bool is_deserialize_buffer_v = is_deserialize_buffer<T>::value;

template <typename T, size_t buf_size>
struct SerializeBuffer {
    static const size_t size_bytes = sizeof(T) * buf_size;
    T buffer[buf_size]{};
};

template <typename T>
struct is_serialize_buffer : std::false_type {};

template <typename T, size_t buf_size>
struct is_serialize_buffer<SerializeBuffer<T, buf_size>> : std::true_type {};

template <typename T>
inline constexpr bool is_serialize_buffer_v = is_serialize_buffer<T>::value;

template <typename... Args>
void serialize(std::vector<uint8_t> &buffer, Args &&...items) {
    auto process = [&](auto &&item) {
        if constexpr (!is_serialize_buffer_v<decltype(item)>) {
            const auto *val = reinterpret_cast<const uint8_t *>(&item);
            buffer.insert(buffer.end(), val, val + sizeof(item));
        } else {
            const auto *val = reinterpret_cast<const uint8_t *>(item.buffer);
            buffer.insert(buffer.end(), val, val + item.size_bytes);
        }
    };

    (process(std::forward<Args>(items)), ...);
}

template <typename... Args>
void deserialize(const uint8_t *buffer, Args &&...items) {
    auto process = [&](auto &&item) {
        if constexpr (!is_deserialize_buffer_v<decltype(item)>) {
            std::memcpy(&item, buffer, sizeof(item));
            buffer += sizeof(item);
        } else {
            std::memcpy(item.buffer, buffer, sizeof(item.buffer));
            buffer += sizeof(item.buffer);
        }
    };

    (process(std::forward<Args>(items)), ...);
}

} // namespace ares

#endif // ARES_SERIALIZATION_HPP
