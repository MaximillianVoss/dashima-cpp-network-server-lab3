#pragma once

#include "network_server/container/AdditiveHash.hpp"
#include "network_server/model/PacketTypes.hpp"

#include <cstddef>
#include <string>

namespace network_server::model {

/**
 * @brief Key for locating packets by recipient address and packet type.
 */
struct PacketKey {
    std::string recipientAddress;
    PacketType packetType;

    /**
     * @brief Compares packet keys.
     */
    friend bool operator==(const PacketKey& left, const PacketKey& right) noexcept {
        return left.packetType == right.packetType &&
               left.recipientAddress == right.recipientAddress;
    }
};

/**
 * @brief Additive hash functor for PacketKey.
 */
class PacketKeyHash {
public:
    /**
     * @brief Returns an additive hash value for a packet key.
     */
    std::size_t operator()(const PacketKey& key) const noexcept {
        auto hash = network_server::container::additiveMixBytes(key.recipientAddress);
        hash += static_cast<std::size_t>(key.packetType) + 0x9E3779B97F4A7C15ULL;
        hash += hash << 6U;
        hash ^= hash >> 2U;
        return hash;
    }
};

}  // namespace network_server::model
