#pragma once

#include "network_server/model/LinkDescriptor.hpp"
#include "network_server/model/Packet.hpp"

#include <cstddef>
#include <vector>

namespace network_server::model {

/**
 * @brief Descriptor of a hypertext packet.
 */
class HypertextPacket final : public Packet {
public:
    /**
     * @brief Creates a hypertext packet descriptor.
     */
    HypertextPacket(
        std::string senderAddress,
        std::string recipientAddress,
        CodeType codeType,
        InfoType infoType,
        MessageDescriptor message,
        std::vector<LinkDescriptor> links);

    /**
     * @brief Returns the packet type.
     */
    PacketType type() const noexcept override;

    /**
     * @brief Returns all hypertext packet information as text.
     */
    std::string describe() const override;

    /**
     * @brief Returns the code type.
     */
    CodeType codeType() const noexcept;

    /**
     * @brief Returns the information type.
     */
    InfoType infoType() const noexcept;

    /**
     * @brief Returns the number of links.
     */
    std::size_t linkCount() const noexcept;

    /**
     * @brief Returns link descriptors.
     */
    const std::vector<LinkDescriptor>& links() const noexcept;

private:
    CodeType codeType_;
    InfoType infoType_;
    std::vector<LinkDescriptor> links_;
};

}  // namespace network_server::model
