#pragma once

#include "network_server/model/LinkDescriptor.hpp"
#include "network_server/model/Packet.hpp"

#include <memory>
#include <vector>

namespace network_server::model {

class HypertextPacket;

/**
 * @brief Descriptor of a file packet.
 */
class FilePacket final : public Packet {
public:
    /**
     * @brief Creates a file packet descriptor.
     */
    FilePacket(
        std::string senderAddress,
        std::string recipientAddress,
        CodeType codeType,
        InfoType infoType,
        MessageDescriptor message);

    /**
     * @brief Returns the packet type.
     */
    PacketType type() const noexcept override;

    /**
     * @brief Returns all file packet information as text.
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
     * @brief Converts this file packet into a hypertext packet.
     */
    std::shared_ptr<HypertextPacket> toHypertextPacket(
        std::vector<LinkDescriptor> links = {}) const;

private:
    CodeType codeType_;
    InfoType infoType_;
};

}  // namespace network_server::model
