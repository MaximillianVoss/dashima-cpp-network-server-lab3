#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

namespace network_server::container {

/**
 * @brief Calculates a byte-wise hash by additive mixing.
 *
 * The algorithm folds every byte into the accumulator by addition and then
 * applies lightweight bit mixing. It is intentionally simple because the lab
 * variant requires an addition-mixed table.
 */
inline std::size_t additiveMixBytes(std::string_view bytes) noexcept {
    std::size_t hash = 0;
    for (const unsigned char byte : bytes) {
        hash += byte;
        hash += hash << 10U;
        hash ^= hash >> 6U;
    }
    hash += hash << 3U;
    hash ^= hash >> 11U;
    hash += hash << 15U;
    return hash;
}

/**
 * @brief Default additive hash functor for string-like and arithmetic keys.
 *
 * Custom composite keys can provide their own functor and still use the same
 * hash table implementation.
 */
template <typename T>
class AdditiveHash {
public:
    /**
     * @brief Returns an additive hash value for the key.
     */
    std::size_t operator()(const T& key) const noexcept {
        if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
            auto value = static_cast<std::uint64_t>(key);
            std::string bytes(sizeof(value), '\0');
            for (std::size_t i = 0; i < sizeof(value); ++i) {
                bytes[i] = static_cast<char>((value >> (i * 8U)) & 0xFFU);
            }
            return additiveMixBytes(bytes);
        } else {
            return additiveMixBytes(std::string_view{key});
        }
    }
};

template <>
class AdditiveHash<std::string> {
public:
    /**
     * @brief Returns an additive hash value for a string.
     */
    std::size_t operator()(const std::string& key) const noexcept {
        return additiveMixBytes(key);
    }
};

}  // namespace network_server::container
