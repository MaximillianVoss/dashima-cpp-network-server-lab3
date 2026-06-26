#pragma once

#include "network_server/model/ServerDescriptor.hpp"
#include "network_server/view/ConsoleView.hpp"

#include <memory>

namespace network_server::controller {

/**
 * @brief Controller for the dialog server application.
 */
class ServerController {
public:
    /**
     * @brief Creates a controller.
     */
    ServerController(
        network_server::model::ServerDescriptor server,
        network_server::view::ConsoleView view);

    /**
     * @brief Runs the dialog loop.
     */
    void run();

private:
    std::shared_ptr<network_server::model::Packet> readPacketByKind(
        network_server::model::PacketType type) const;
    void addPacket();
    void findPacket() const;
    void deletePacket();
    void selectPacket();
    void convertPacket();
    void showTable() const;
    void showSenders() const;
    void showPercentages() const;

    network_server::model::ServerDescriptor server_;
    network_server::view::ConsoleView view_;
};

}  // namespace network_server::controller
