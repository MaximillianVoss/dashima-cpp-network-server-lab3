#pragma once

#include "network_server/model/PacketTypes.hpp"

#include <string>

namespace network_server::model {

/**
 * @brief Descriptor of a hypertext link.
 */
class LinkDescriptor {
public:
    /**
     * @brief Creates a link descriptor.
     */
    LinkDescriptor(ProtocolType protocol, std::string serverName);

    /**
     * @brief Returns the link protocol.
     */
    ProtocolType protocol() const noexcept;

    /**
     * @brief Returns the network name of the linked server.
     */
    const std::string& serverName() const noexcept;

    /**
     * @brief Returns a formatted link description.
     */
    std::string describe() const;

private:
    ProtocolType protocol_;
    std::string serverName_;
};

}  // namespace network_server::model
