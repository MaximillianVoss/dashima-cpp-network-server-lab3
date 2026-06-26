#pragma once

#include "network_server/model/PacketStatistics.hpp"
#include "network_server/model/TransmissionTable.hpp"

#include <memory>
#include <string>
#include <vector>

namespace network_server::model {

/**
 * @brief Complete server descriptor with identity and transmission table.
 */
class ServerDescriptor {
public:
    /**
     * @brief Creates a server descriptor.
     */
    ServerDescriptor(std::string networkName, std::string networkAddress);

    /**
     * @brief Returns the server network name.
     */
    const std::string& networkName() const noexcept;

    /**
     * @brief Returns the server network address.
     */
    const std::string& networkAddress() const noexcept;

    /**
     * @brief Returns the transmission table.
     */
    TransmissionTable& transmissions() noexcept;

    /**
     * @brief Returns the transmission table.
     */
    const TransmissionTable& transmissions() const noexcept;

    /**
     * @brief Adds a packet into the transmission table.
     */
    bool addPacket(std::shared_ptr<Packet> packet);

    /**
     * @brief Finds a packet by recipient address and type.
     */
    std::shared_ptr<Packet> findPacket(const std::string& recipientAddress, PacketType type) const;

    /**
     * @brief Deletes a packet by recipient address and type.
     */
    bool deletePacket(const std::string& recipientAddress, PacketType type);

    /**
     * @brief Selects and removes a packet by recipient with priority hypertext, file, mail.
     */
    std::shared_ptr<Packet> selectByRecipientPriority(const std::string& recipientAddress);

    /**
     * @brief Returns sender addresses for all packets in the table.
     */
    std::vector<std::string> senderAddresses() const;

    /**
     * @brief Calculates packet type percentages in one thread.
     */
    PacketPercentages percentagesSequential() const;

    /**
     * @brief Calculates packet type percentages in multiple threads.
     */
    PacketPercentages percentagesParallel() const;

    /**
     * @brief Returns a formatted server description.
     */
    std::string describe() const;

private:
    std::string networkName_;
    std::string networkAddress_;
    TransmissionTable transmissions_;
};

}  // namespace network_server::model
