#pragma once

#include "network_server/model/Packet.hpp"

#include <memory>
#include <string>

namespace network_server::model {

class FilePacket;

/**
 * @brief Descriptor of a mail packet.
 */
class MailPacket final : public Packet {
public:
    /**
     * @brief Creates a mail packet descriptor.
     */
    MailPacket(
        std::string senderAddress,
        std::string recipientAddress,
        std::string userName,
        MessageDescriptor message);

    /**
     * @brief Returns the packet type.
     */
    PacketType type() const noexcept override;

    /**
     * @brief Returns all mail packet information as text.
     */
    std::string describe() const override;

    /**
     * @brief Returns the user name.
     */
    const std::string& userName() const noexcept;

    /**
     * @brief Converts this mail packet into a file packet.
     */
    std::shared_ptr<FilePacket> toFilePacket(
        CodeType codeType = CodeType::ASCII,
        InfoType infoType = InfoType::Data) const;

private:
    std::string userName_;
};

}  // namespace network_server::model
