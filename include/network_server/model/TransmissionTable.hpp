#pragma once

#include "network_server/container/HashTable.hpp"
#include "network_server/model/Packet.hpp"
#include "network_server/model/PacketKey.hpp"

#include <memory>
#include <string>

namespace network_server::model {

/**
 * @brief Addition-mixed transmission table storing packet descriptors.
 */
class TransmissionTable {
public:
    using Table = network_server::container::HashTable<
        PacketKey,
        std::shared_ptr<Packet>,
        PacketKeyHash>;
    using iterator = Table::iterator;
    using const_iterator = Table::const_iterator;

    /**
     * @brief Creates an empty transmission table.
     */
    TransmissionTable() = default;

    /**
     * @brief Inserts a packet descriptor into the table.
     *
     * Existing packets with the same recipient and type are replaced.
     */
    bool insert(std::shared_ptr<Packet> packet);

    /**
     * @brief Finds a packet by recipient address and packet type.
     */
    std::shared_ptr<Packet> find(const std::string& recipientAddress, PacketType type) const;

    /**
     * @brief Removes a packet by recipient address and packet type.
     */
    bool remove(const std::string& recipientAddress, PacketType type);

    /**
     * @brief Returns all table entries as text.
     */
    std::string describe() const;

    /**
     * @brief Returns the number of stored packets.
     */
    std::size_t size() const noexcept;

    /**
     * @brief Returns true if the table contains no packets.
     */
    bool empty() const noexcept;

    /**
     * @brief Returns an iterator to the first table entry.
     */
    iterator begin() noexcept;

    /**
     * @brief Returns an iterator past the last table entry.
     */
    iterator end() noexcept;

    /**
     * @brief Returns a const iterator to the first table entry.
     */
    const_iterator begin() const noexcept;

    /**
     * @brief Returns a const iterator past the last table entry.
     */
    const_iterator end() const noexcept;

private:
    Table table_;
};

}  // namespace network_server::model
