#include "network_server/model/MessageDescriptor.hpp"

#include <utility>

namespace network_server::model {

MessageDescriptor::MessageDescriptor(std::string content) : content_(std::move(content)) {}

std::size_t MessageDescriptor::length() const noexcept {
    return content_.size();
}

const std::string& MessageDescriptor::content() const noexcept {
    return content_;
}

}  // namespace network_server::model
