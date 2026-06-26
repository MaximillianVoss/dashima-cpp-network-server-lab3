#include "network_server/model/Packet.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace network_server::model {

Packet::Packet(
    std::string senderAddress,
    std::string recipientAddress,
    MessageDescriptor message)
    : senderAddress_(std::move(senderAddress)),
      recipientAddress_(std::move(recipientAddress)),
      message_(std::move(message)) {
    if (senderAddress_.empty()) {
        throw std::invalid_argument("Sender address cannot be empty");
    }
    if (recipientAddress_.empty()) {
        throw std::invalid_argument("Recipient address cannot be empty");
    }
}

const std::string& Packet::senderAddress() const noexcept {
    return senderAddress_;
}

const std::string& Packet::recipientAddress() const noexcept {
    return recipientAddress_;
}

const MessageDescriptor& Packet::message() const noexcept {
    return message_;
}

std::string Packet::describeCommon() const {
    std::ostringstream output;
    output << "type: " << toString(type()) << '\n'
           << "sender: " << senderAddress_ << '\n'
           << "recipient: " << recipientAddress_ << '\n'
           << "message length: " << message_.length() << '\n'
           << "message: " << message_.content() << '\n';
    return output.str();
}

}  // namespace network_server::model
