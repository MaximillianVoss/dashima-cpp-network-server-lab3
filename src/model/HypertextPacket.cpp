#include "network_server/model/HypertextPacket.hpp"

#include <sstream>
#include <utility>

namespace network_server::model {

HypertextPacket::HypertextPacket(
    std::string senderAddress,
    std::string recipientAddress,
    CodeType codeType,
    InfoType infoType,
    MessageDescriptor message,
    std::vector<LinkDescriptor> links)
    : Packet(std::move(senderAddress), std::move(recipientAddress), std::move(message)),
      codeType_(codeType),
      infoType_(infoType),
      links_(std::move(links)) {}

PacketType HypertextPacket::type() const noexcept {
    return PacketType::Hypertext;
}

std::string HypertextPacket::describe() const {
    std::ostringstream output;
    output << describeCommon()
           << "code type: " << toString(codeType_) << '\n'
           << "information type: " << toString(infoType_) << '\n'
           << "links: " << links_.size() << '\n';

    for (const auto& link : links_) {
        output << "  - " << link.describe() << '\n';
    }
    return output.str();
}

CodeType HypertextPacket::codeType() const noexcept {
    return codeType_;
}

InfoType HypertextPacket::infoType() const noexcept {
    return infoType_;
}

std::size_t HypertextPacket::linkCount() const noexcept {
    return links_.size();
}

const std::vector<LinkDescriptor>& HypertextPacket::links() const noexcept {
    return links_;
}

}  // namespace network_server::model
