#include "network_server/controller/ServerController.hpp"
#include "network_server/model/ServerDescriptor.hpp"
#include "network_server/view/ConsoleView.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        network_server::model::ServerDescriptor server{"study.server.local", "127.0.0.1"};
        network_server::view::ConsoleView view{std::cin, std::cout};
        network_server::controller::ServerController controller{std::move(server), view};
        controller.run();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
