#include "network_server/model/TransmissionTable.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace network_server::model {

bool TransmissionTable::insert(std::shared_ptr<Packet> packet) {
    if (packet == nullptr) {
        throw std::invalid_argument("Packet cannot be null");
    }
    PacketKey key{packet->recipientAddress(), packet->type()};
    return table_.insert(std::move(key), std::move(packet));
}

std::shared_ptr<Packet> TransmissionTable::find(
    const std::string& recipientAddress,
    PacketType type) const {
    const auto packet = table_.find(PacketKey{recipientAddress, type});
    return packet == nullptr ? nullptr : *packet;
}

bool TransmissionTable::remove(const std::string& recipientAddress, PacketType type) {
    return table_.erase(PacketKey{recipientAddress, type});
}

std::string TransmissionTable::describe() const {
    if (table_.empty()) {
        return "Transmission table is empty.\n";
    }

    std::ostringstream output;
    output << "Transmission table (" << table_.size() << " packets):\n";
    for (const auto& entry : table_) {
        output << "------------------------------\n" << entry.value->describe();
    }
    return output.str();
}

std::size_t TransmissionTable::size() const noexcept {
    return table_.size();
}

bool TransmissionTable::empty() const noexcept {
    return table_.empty();
}

TransmissionTable::iterator TransmissionTable::begin() noexcept {
    return table_.begin();
}

TransmissionTable::iterator TransmissionTable::end() noexcept {
    return table_.end();
}

TransmissionTable::const_iterator TransmissionTable::begin() const noexcept {
    return table_.begin();
}

TransmissionTable::const_iterator TransmissionTable::end() const noexcept {
    return table_.end();
}

}  // namespace network_server::model
