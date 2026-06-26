#include "network_server/view/ConsoleView.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace network_server::view {

using network_server::model::CodeType;
using network_server::model::InfoType;
using network_server::model::LinkDescriptor;
using network_server::model::PacketPercentages;
using network_server::model::PacketType;
using network_server::model::ProtocolType;
using network_server::model::codeTypeFromMenu;
using network_server::model::infoTypeFromMenu;
using network_server::model::packetTypeFromMenu;
using network_server::model::protocolTypeFromMenu;

ConsoleView::ConsoleView(std::istream& input, std::ostream& output)
    : input_(input), output_(output) {}

void ConsoleView::showMenu() const {
    output_ << "\nNetwork server dialog\n"
            << "1. Add packet\n"
            << "2. Find packet by recipient and type\n"
            << "3. Delete packet by recipient and type\n"
            << "4. Select packet by recipient priority\n"
            << "5. Convert packet\n"
            << "6. Show transmission table\n"
            << "7. Show senders\n"
            << "8. Show packet type percentages\n"
            << "0. Exit\n";
}

void ConsoleView::showMessage(const std::string& message) const {
    output_ << message;
    if (!message.empty() && message.back() != '\n') {
        output_ << '\n';
    }
}

void ConsoleView::showPercentages(const PacketPercentages& percentages) const {
    output_ << "Total packets: " << percentages.total << '\n'
            << "mail: " << percentages.mail << "%\n"
            << "file: " << percentages.file << "%\n"
            << "hypertext: " << percentages.hypertext << "%\n";
}

std::string ConsoleView::readLine(const std::string& prompt) const {
    output_ << prompt;
    std::string line;
    std::getline(input_, line);
    if (!input_) {
        throw std::runtime_error("Input stream was closed");
    }
    return line;
}

int ConsoleView::readInt(const std::string& prompt) const {
    while (true) {
        const auto line = readLine(prompt);
        std::istringstream parser(line);
        int value = 0;
        if (parser >> value) {
            return value;
        }
        output_ << "Enter a valid integer.\n";
    }
}

PacketType ConsoleView::readPacketType() const {
    while (true) {
        output_ << "Packet type: 1 - mail, 2 - file, 3 - hypertext\n";
        try {
            return packetTypeFromMenu(readInt("> "));
        } catch (const std::invalid_argument&) {
            output_ << "Unknown packet type.\n";
        }
    }
}

CodeType ConsoleView::readCodeType() const {
    while (true) {
        output_ << "Code type: 1 - ASCII, 2 - BIN\n";
        try {
            return codeTypeFromMenu(readInt("> "));
        } catch (const std::invalid_argument&) {
            output_ << "Unknown code type.\n";
        }
    }
}

InfoType ConsoleView::readInfoType() const {
    while (true) {
        output_ << "Information type: 1 - control, 2 - data\n";
        try {
            return infoTypeFromMenu(readInt("> "));
        } catch (const std::invalid_argument&) {
            output_ << "Unknown information type.\n";
        }
    }
}

ProtocolType ConsoleView::readProtocolType() const {
    while (true) {
        output_ << "Protocol: 1 - FTP, 2 - HTTP\n";
        try {
            return protocolTypeFromMenu(readInt("> "));
        } catch (const std::invalid_argument&) {
            output_ << "Unknown protocol type.\n";
        }
    }
}

std::vector<LinkDescriptor> ConsoleView::readLinks() const {
    std::vector<LinkDescriptor> links;
    const auto count = readInt("Link count: ");
    if (count < 0) {
        throw std::invalid_argument("Link count cannot be negative");
    }

    for (int i = 0; i < count; ++i) {
        output_ << "Link #" << (i + 1) << '\n';
        const auto protocol = readProtocolType();
        auto serverName = readLine("Linked server name: ");
        links.emplace_back(protocol, std::move(serverName));
    }
    return links;
}

}  // namespace network_server::view
