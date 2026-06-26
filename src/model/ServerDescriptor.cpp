#include "network_server/model/ServerDescriptor.hpp"

#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace network_server::model {

ServerDescriptor::ServerDescriptor(std::string networkName, std::string networkAddress)
    : networkName_(std::move(networkName)), networkAddress_(std::move(networkAddress)) {
    if (networkName_.empty()) {
        throw std::invalid_argument("Server network name cannot be empty");
    }
    if (networkAddress_.empty()) {
        throw std::invalid_argument("Server network address cannot be empty");
    }
}

const std::string& ServerDescriptor::networkName() const noexcept {
    return networkName_;
}

const std::string& ServerDescriptor::networkAddress() const noexcept {
    return networkAddress_;
}

TransmissionTable& ServerDescriptor::transmissions() noexcept {
    return transmissions_;
}

const TransmissionTable& ServerDescriptor::transmissions() const noexcept {
    return transmissions_;
}

bool ServerDescriptor::addPacket(std::shared_ptr<Packet> packet) {
    return transmissions_.insert(std::move(packet));
}

std::shared_ptr<Packet> ServerDescriptor::findPacket(
    const std::string& recipientAddress,
    PacketType type) const {
    return transmissions_.find(recipientAddress, type);
}

bool ServerDescriptor::deletePacket(const std::string& recipientAddress, PacketType type) {
    return transmissions_.remove(recipientAddress, type);
}

std::shared_ptr<Packet> ServerDescriptor::selectByRecipientPriority(
    const std::string& recipientAddress) {
    constexpr PacketType priority[] = {
        PacketType::Hypertext,
        PacketType::File,
        PacketType::Mail};

    for (const auto type : priority) {
        auto packet = transmissions_.find(recipientAddress, type);
        if (packet != nullptr) {
            transmissions_.remove(recipientAddress, type);
            return packet;
        }
    }
    return nullptr;
}

std::vector<std::string> ServerDescriptor::senderAddresses() const {
    std::set<std::string> uniqueSenders;
    for (const auto& entry : transmissions_) {
        uniqueSenders.insert(entry.value->senderAddress());
    }
    return {uniqueSenders.begin(), uniqueSenders.end()};
}

PacketPercentages ServerDescriptor::percentagesSequential() const {
    return PacketStatistics::calculateSequential(transmissions_);
}

PacketPercentages ServerDescriptor::percentagesParallel() const {
    return PacketStatistics::calculateParallel(transmissions_);
}

std::string ServerDescriptor::describe() const {
    std::ostringstream output;
    output << "Server name: " << networkName_ << '\n'
           << "Server address: " << networkAddress_ << '\n'
           << transmissions_.describe();
    return output.str();
}

}  // namespace network_server::model
