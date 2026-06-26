#pragma once

#include "network_server/model/MessageDescriptor.hpp"
#include "network_server/model/PacketTypes.hpp"

#include <string>

namespace network_server::model {

/**
 * @brief Base class for every packet descriptor.
 */
class Packet {
public:
    /**
     * @brief Creates a packet with sender, recipient and message descriptors.
     */
    Packet(std::string senderAddress, std::string recipientAddress, MessageDescriptor message);

    /**
     * @brief Destroys the packet polymorphically.
     */
    virtual ~Packet() = default;

    Packet(const Packet&) = default;
    Packet& operator=(const Packet&) = default;
    Packet(Packet&&) noexcept = default;
    Packet& operator=(Packet&&) noexcept = default;

    /**
     * @brief Returns the concrete packet type.
     */
    virtual PacketType type() const noexcept = 0;

    /**
     * @brief Returns all packet information as text.
     */
    virtual std::string describe() const = 0;

    /**
     * @brief Returns the sender network address.
     */
    const std::string& senderAddress() const noexcept;

    /**
     * @brief Returns the recipient network address.
     */
    const std::string& recipientAddress() const noexcept;

    /**
     * @brief Returns the message descriptor.
     */
    const MessageDescriptor& message() const noexcept;

protected:
    /**
     * @brief Builds the common part of a packet description.
     */
    std::string describeCommon() const;

private:
    std::string senderAddress_;
    std::string recipientAddress_;
    MessageDescriptor message_;
};

}  // namespace network_server::model
