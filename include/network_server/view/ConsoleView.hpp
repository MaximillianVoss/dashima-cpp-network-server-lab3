#pragma once

#include "network_server/model/LinkDescriptor.hpp"
#include "network_server/model/PacketStatistics.hpp"
#include "network_server/model/PacketTypes.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace network_server::view {

/**
 * @brief Console view for the dialog application.
 */
class ConsoleView {
public:
    /**
     * @brief Creates a console view bound to streams.
     */
    ConsoleView(std::istream& input, std::ostream& output);

    /**
     * @brief Shows the main menu.
     */
    void showMenu() const;

    /**
     * @brief Prints a text message.
     */
    void showMessage(const std::string& message) const;

    /**
     * @brief Prints packet percentages.
     */
    void showPercentages(const network_server::model::PacketPercentages& percentages) const;

    /**
     * @brief Reads a line with a prompt.
     */
    std::string readLine(const std::string& prompt) const;

    /**
     * @brief Reads an integer with a prompt.
     */
    int readInt(const std::string& prompt) const;

    /**
     * @brief Reads a packet type from the dialog.
     */
    network_server::model::PacketType readPacketType() const;

    /**
     * @brief Reads a code type from the dialog.
     */
    network_server::model::CodeType readCodeType() const;

    /**
     * @brief Reads an information type from the dialog.
     */
    network_server::model::InfoType readInfoType() const;

    /**
     * @brief Reads a protocol type from the dialog.
     */
    network_server::model::ProtocolType readProtocolType() const;

    /**
     * @brief Reads hypertext link descriptors from the dialog.
     */
    std::vector<network_server::model::LinkDescriptor> readLinks() const;

private:
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace network_server::view
