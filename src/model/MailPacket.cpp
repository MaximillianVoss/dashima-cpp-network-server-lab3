#include "network_server/model/MailPacket.hpp"

#include "network_server/model/FilePacket.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace network_server::model {

MailPacket::MailPacket(
    std::string senderAddress,
    std::string recipientAddress,
    std::string userName,
    MessageDescriptor message)
    : Packet(std::move(senderAddress), std::move(recipientAddress), std::move(message)),
      userName_(std::move(userName)) {
    if (userName_.empty()) {
        throw std::invalid_argument("User name cannot be empty");
    }
    if (userName_.size() > 20) {
        throw std::invalid_argument("User name cannot be longer than 20 characters");
    }
}

PacketType MailPacket::type() const noexcept {
    return PacketType::Mail;
}

std::string MailPacket::describe() const {
    std::ostringstream output;
    output << describeCommon() << "user name: " << userName_ << '\n';
    return output.str();
}

const std::string& MailPacket::userName() const noexcept {
    return userName_;
}

std::shared_ptr<FilePacket> MailPacket::toFilePacket(CodeType codeType, InfoType infoType) const {
    return std::make_shared<FilePacket>(
        senderAddress(),
        recipientAddress(),
        codeType,
        infoType,
        MessageDescriptor{message().content()});
}

}  // namespace network_server::model
