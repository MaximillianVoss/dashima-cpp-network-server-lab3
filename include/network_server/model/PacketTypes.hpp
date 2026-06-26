#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace network_server::model {

/**
 * @brief Supported packet descriptor kinds.
 */
enum class PacketType {
    Mail,
    File,
    Hypertext
};

/**
 * @brief Message code type.
 */
enum class CodeType {
    ASCII,
    BIN
};

/**
 * @brief Message information type.
 */
enum class InfoType {
    Control,
    Data
};

/**
 * @brief Hyperlink protocol type.
 */
enum class ProtocolType {
    FTP,
    HTTP
};

/**
 * @brief Converts a packet type to a readable string.
 */
std::string_view toString(PacketType type) noexcept;

/**
 * @brief Converts a code type to a readable string.
 */
std::string_view toString(CodeType type) noexcept;

/**
 * @brief Converts an information type to a readable string.
 */
std::string_view toString(InfoType type) noexcept;

/**
 * @brief Converts a protocol type to a readable string.
 */
std::string_view toString(ProtocolType type) noexcept;

/**
 * @brief Parses a numeric menu value into a packet type.
 */
PacketType packetTypeFromMenu(int value);

/**
 * @brief Parses a numeric menu value into a code type.
 */
CodeType codeTypeFromMenu(int value);

/**
 * @brief Parses a numeric menu value into an information type.
 */
InfoType infoTypeFromMenu(int value);

/**
 * @brief Parses a numeric menu value into a protocol type.
 */
ProtocolType protocolTypeFromMenu(int value);

}  // namespace network_server::model
