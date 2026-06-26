#include "network_server/model/LinkDescriptor.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace network_server::model {

LinkDescriptor::LinkDescriptor(ProtocolType protocol, std::string serverName)
    : protocol_(protocol), serverName_(std::move(serverName)) {
    if (serverName_.empty()) {
        throw std::invalid_argument("Link server name cannot be empty");
    }
}

ProtocolType LinkDescriptor::protocol() const noexcept {
    return protocol_;
}

const std::string& LinkDescriptor::serverName() const noexcept {
    return serverName_;
}

std::string LinkDescriptor::describe() const {
    std::ostringstream output;
    output << toString(protocol_) << "://" << serverName_;
    return output.str();
}

}  // namespace network_server::model
