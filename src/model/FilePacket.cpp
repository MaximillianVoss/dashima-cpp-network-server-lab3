#include "network_server/model/FilePacket.hpp"

#include "network_server/model/HypertextPacket.hpp"

#include <sstream>
#include <utility>

namespace network_server::model {

FilePacket::FilePacket(
    std::string senderAddress,
    std::string recipientAddress,
    CodeType codeType,
    InfoType infoType,
    MessageDescriptor message)
    : Packet(std::move(senderAddress), std::move(recipientAddress), std::move(message)),
      codeType_(codeType),
      infoType_(infoType) {}

PacketType FilePacket::type() const noexcept {
    return PacketType::File;
}

std::string FilePacket::describe() const {
    std::ostringstream output;
    output << describeCommon()
           << "code type: " << toString(codeType_) << '\n'
           << "information type: " << toString(infoType_) << '\n';
    return output.str();
}

CodeType FilePacket::codeType() const noexcept {
    return codeType_;
}

InfoType FilePacket::infoType() const noexcept {
    return infoType_;
}

std::shared_ptr<HypertextPacket> FilePacket::toHypertextPacket(
    std::vector<LinkDescriptor> links) const {
    return std::make_shared<HypertextPacket>(
        senderAddress(),
        recipientAddress(),
        codeType_,
        infoType_,
        MessageDescriptor{message().content()},
        std::move(links));
}

}  // namespace network_server::model
