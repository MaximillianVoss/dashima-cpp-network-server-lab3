#include "network_server/model/PacketTypes.hpp"

namespace network_server::model {

std::string_view toString(PacketType type) noexcept {
    switch (type) {
        case PacketType::Mail:
            return "mail";
        case PacketType::File:
            return "file";
        case PacketType::Hypertext:
            return "hypertext";
    }
    return "unknown";
}

std::string_view toString(CodeType type) noexcept {
    switch (type) {
        case CodeType::ASCII:
            return "ASCII";
        case CodeType::BIN:
            return "BIN";
    }
    return "unknown";
}

std::string_view toString(InfoType type) noexcept {
    switch (type) {
        case InfoType::Control:
            return "control";
        case InfoType::Data:
            return "data";
    }
    return "unknown";
}

std::string_view toString(ProtocolType type) noexcept {
    switch (type) {
        case ProtocolType::FTP:
            return "FTP";
        case ProtocolType::HTTP:
            return "HTTP";
    }
    return "unknown";
}

PacketType packetTypeFromMenu(int value) {
    switch (value) {
        case 1:
            return PacketType::Mail;
        case 2:
            return PacketType::File;
        case 3:
            return PacketType::Hypertext;
        default:
            throw std::invalid_argument("Unknown packet type menu value");
    }
}

CodeType codeTypeFromMenu(int value) {
    switch (value) {
        case 1:
            return CodeType::ASCII;
        case 2:
            return CodeType::BIN;
        default:
            throw std::invalid_argument("Unknown code type menu value");
    }
}

InfoType infoTypeFromMenu(int value) {
    switch (value) {
        case 1:
            return InfoType::Control;
        case 2:
            return InfoType::Data;
        default:
            throw std::invalid_argument("Unknown information type menu value");
    }
}

ProtocolType protocolTypeFromMenu(int value) {
    switch (value) {
        case 1:
            return ProtocolType::FTP;
        case 2:
            return ProtocolType::HTTP;
        default:
            throw std::invalid_argument("Unknown protocol type menu value");
    }
}

}  // namespace network_server::model
