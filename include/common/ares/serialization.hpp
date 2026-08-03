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
/**
 * Set a bit.
 * @tparam T Type of the bitfield.
 * @param bitfield The bitfield to modify.
 * @param bit The bit to modify.
 * @param value @p true to set the bit, @p false to clear the bit.
 */
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

/**
 * Clear a bit.
 * @tparam T Type of the bitfield.
 * @param bitfield The bitfield to check.
 * @param bit The bit to check.
 * @return @p true if the bit is set, @p false if the bit is clear.
 */
template <typename T>
bool check_bit(const T bitfield, size_t bit) {
    constexpr size_t T_size = sizeof(T) * __CHAR_BIT__;
    assert(bit <= T_size);

    T mask = static_cast<T>(1) << bit;
    return (bitfield & mask) != static_cast<T>(0);
}

/**
 * Set a bitfield.
 * @tparam T1 Type of the bitfield container.
 * @tparam T2 Type of the value.
 * @param bitfield The bitfield container to modify.
 * @param lsb The least significant bit of the bitfield.
 * @param num_bits The number of bits in the bitfield.
 * @param value The value of the bitfield.
 */
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

/**
 * Retrieve a bitfield value.
 * @tparam T1 Type of the bitfield container.
 * @tparam T2 Type of the bitfield value.
 * @param bitfield The bitfield container to check or read from.
 * @param lsb The least significant bit in the bitfield.
 * @param num_bits The number of bits in the bitfield.
 * @param value The value of the bitfield.
 * @return The value of the bitfield.
 */
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

/**
 * Set flags in a bitfield container starting from the bit specified.
 * @tparam start_bit The bit to start setting flags at.
 * @tparam T The bitfield container type.
 * @tparam Args The variadic argument types.
 * @param bitfield The bitfield container to set the flags in.
 * @param flags The flags to set, starting from the bit specified in start_bit.
 * All flag types must be of type @p bool.
 */
template <size_t start_bit = 0, typename T, typename... Args>
void set_flags(T &bitfield, Args... flags) {
    constexpr size_t T_size = sizeof(T) * __CHAR_BIT__;
    static_assert(start_bit < T_size);
    static_assert(sizeof...(Args) <= (T_size - start_bit));
    static_assert(std::conjunction_v<std::is_same<Args, bool>...>);

    size_t bit = start_bit;

    auto process = [&](auto &&flag) {
        set_bit(bitfield, bit, flag);
        ++bit;
    };

    (process(std::forward<Args>(flags)), ...);
}

/**
 * Read flags from a bitfield container starting from the bit specified.
 * @tparam start_bit The bit to start reading flags from.
 * @tparam T The bitfield type.
 * @tparam Args The variadic argument types.
 * @param bitfield The bitfield container to get the flags from.
 * @param flags The flags to be read, starting from the start_bit. All flag
 * types must be of type @p bool&.
 */
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

/**
 *@struct DeserializeBuffer
 * Deserialize buffer struct.
 * @tparam T The type of the buffer.
 * @tparam buf_size The amount of buffer items there are.
 *
 * @note The recommended use case for this is deserializing reserved fields.
 */
template <typename T, size_t buf_size>
struct DeserializeBuffer {
    /**
     * The internal buffer.
     */
    T buffer[buf_size]{};
};

template <typename T>
struct is_deserialize_buffer : std::false_type {};

template <typename T, size_t buf_size>
struct is_deserialize_buffer<DeserializeBuffer<T, buf_size>> : std::true_type {
};

template <typename T>
inline constexpr bool is_deserialize_buffer_v = is_deserialize_buffer<T>::value;

/**
 * @struct SerializeBuffer
 * Serialize buffer struct.
 * @tparam T The type of the buffer.
 * @tparam buf_size The buffer size.
 *
 * @note The recommended use case for this is serializing reserved fields.
 */
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

/**
 * Serialize items into a buffer.
 * @tparam Args The variadic argument types.
 * @param buffer The buffer to encode into.
 * @param items The items to place into the buffer, starting from the first
 * item.
 */
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

/**
 * Deserialize items from a buffer.
 * @tparam Args The variadic argument types.
 * @param buffer The buffer to decode items from.
 * @param items The items to decode into.
 */
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
