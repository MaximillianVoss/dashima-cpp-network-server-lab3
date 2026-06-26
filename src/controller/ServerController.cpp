#include "network_server/controller/ServerController.hpp"

#include "network_server/model/FilePacket.hpp"
#include "network_server/model/HypertextPacket.hpp"
#include "network_server/model/MailPacket.hpp"

#include <exception>
#include <memory>
#include <sstream>
#include <utility>

namespace network_server::controller {

using network_server::model::CodeType;
using network_server::model::FilePacket;
using network_server::model::HypertextPacket;
using network_server::model::InfoType;
using network_server::model::MailPacket;
using network_server::model::MessageDescriptor;
using network_server::model::Packet;
using network_server::model::PacketType;
using network_server::model::ServerDescriptor;
using network_server::view::ConsoleView;

ServerController::ServerController(ServerDescriptor server, ConsoleView view)
    : server_(std::move(server)), view_(view) {}

void ServerController::run() {
    view_.showMessage(server_.describe());

    while (true) {
        try {
            view_.showMenu();
            const auto command = view_.readInt("> ");
            switch (command) {
                case 1:
                    addPacket();
                    break;
                case 2:
                    findPacket();
                    break;
                case 3:
                    deletePacket();
                    break;
                case 4:
                    selectPacket();
                    break;
                case 5:
                    convertPacket();
                    break;
                case 6:
                    showTable();
                    break;
                case 7:
                    showSenders();
                    break;
                case 8:
                    showPercentages();
                    break;
                case 0:
                    view_.showMessage("Done.");
                    return;
                default:
                    view_.showMessage("Unknown menu command.");
                    break;
            }
        } catch (const std::exception& error) {
            view_.showMessage(std::string{"Error: "} + error.what());
        }
    }
}

std::shared_ptr<Packet> ServerController::readPacketByKind(PacketType type) const {
    const auto sender = view_.readLine("Sender address: ");
    const auto recipient = view_.readLine("Recipient address: ");
    const auto message = MessageDescriptor{view_.readLine("Message: ")};

    switch (type) {
        case PacketType::Mail: {
            auto userName = view_.readLine("User name (up to 20 chars): ");
            return std::make_shared<MailPacket>(sender, recipient, std::move(userName), message);
        }
        case PacketType::File: {
            const auto codeType = view_.readCodeType();
            const auto infoType = view_.readInfoType();
            return std::make_shared<FilePacket>(sender, recipient, codeType, infoType, message);
        }
        case PacketType::Hypertext: {
            const auto codeType = view_.readCodeType();
            const auto infoType = view_.readInfoType();
            auto links = view_.readLinks();
            return std::make_shared<HypertextPacket>(
                sender,
                recipient,
                codeType,
                infoType,
                message,
                std::move(links));
        }
    }
    return nullptr;
}

void ServerController::addPacket() {
    auto packet = readPacketByKind(view_.readPacketType());
    const auto inserted = server_.addPacket(std::move(packet));
    view_.showMessage(inserted ? "Packet added." : "Packet replaced.");
}

void ServerController::findPacket() const {
    const auto recipient = view_.readLine("Recipient address: ");
    const auto type = view_.readPacketType();
    const auto packet = server_.findPacket(recipient, type);
    view_.showMessage(packet == nullptr ? "Packet not found." : packet->describe());
}

void ServerController::deletePacket() {
    const auto recipient = view_.readLine("Recipient address: ");
    const auto type = view_.readPacketType();
    view_.showMessage(server_.deletePacket(recipient, type) ? "Packet deleted." : "Packet not found.");
}

void ServerController::selectPacket() {
    const auto recipient = view_.readLine("Recipient address: ");
    const auto packet = server_.selectByRecipientPriority(recipient);
    view_.showMessage(packet == nullptr ? "Packet not found." : "Selected and removed:\n" + packet->describe());
}

void ServerController::convertPacket() {
    const auto recipient = view_.readLine("Recipient address: ");
    const auto type = view_.readPacketType();
    const auto packet = server_.findPacket(recipient, type);
    if (packet == nullptr) {
        view_.showMessage("Packet not found.");
        return;
    }

    if (type == PacketType::Mail) {
        const auto mail = std::dynamic_pointer_cast<MailPacket>(packet);
        const auto file = mail->toFilePacket(view_.readCodeType(), view_.readInfoType());
        server_.deletePacket(recipient, type);
        server_.addPacket(file);
        view_.showMessage("Mail packet converted to file packet.");
        return;
    }

    if (type == PacketType::File) {
        const auto file = std::dynamic_pointer_cast<FilePacket>(packet);
        auto links = view_.readLinks();
        const auto hypertext = file->toHypertextPacket(std::move(links));
        server_.deletePacket(recipient, type);
        server_.addPacket(hypertext);
        view_.showMessage("File packet converted to hypertext packet.");
        return;
    }

    view_.showMessage("Hypertext packet cannot be converted by this variant.");
}

void ServerController::showTable() const {
    view_.showMessage(server_.describe());
}

void ServerController::showSenders() const {
    const auto senders = server_.senderAddresses();
    if (senders.empty()) {
        view_.showMessage("No senders.");
        return;
    }

    std::ostringstream output;
    output << "Senders:\n";
    for (const auto& sender : senders) {
        output << "- " << sender << '\n';
    }
    view_.showMessage(output.str());
}

void ServerController::showPercentages() const {
    view_.showPercentages(server_.percentagesParallel());
}

}  // namespace network_server::controller
