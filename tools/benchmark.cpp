#include "network_server/model/FilePacket.hpp"
#include "network_server/model/HypertextPacket.hpp"
#include "network_server/model/MailPacket.hpp"
#include "network_server/model/ServerDescriptor.hpp"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using network_server::model::CodeType;
using network_server::model::FilePacket;
using network_server::model::HypertextPacket;
using network_server::model::InfoType;
using network_server::model::MailPacket;
using network_server::model::MessageDescriptor;
using network_server::model::PacketType;
using network_server::model::ServerDescriptor;

namespace {

template <typename Func>
long long measureMicroseconds(Func&& func) {
    const auto started = std::chrono::steady_clock::now();
    func();
    const auto finished = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(finished - started).count();
}

ServerDescriptor createServer(std::size_t packetCount) {
    ServerDescriptor server{"bench.server.local", "10.0.0.1"};
    for (std::size_t i = 0; i < packetCount; ++i) {
        const auto sender = "192.168.1." + std::to_string((i % 250) + 1);
        const auto recipient = "10.10.0." + std::to_string(i + 1);
        const auto message = MessageDescriptor{"payload-" + std::to_string(i)};

        switch (i % 3) {
            case 0:
                server.addPacket(std::make_shared<MailPacket>(sender, recipient, "student", message));
                break;
            case 1:
                server.addPacket(std::make_shared<FilePacket>(
                    sender,
                    recipient,
                    CodeType::ASCII,
                    InfoType::Data,
                    message));
                break;
            default:
                server.addPacket(std::make_shared<HypertextPacket>(
                    sender,
                    recipient,
                    CodeType::BIN,
                    InfoType::Control,
                    message,
                    std::vector<network_server::model::LinkDescriptor>{}));
                break;
        }
    }
    return server;
}

}  // namespace

int main() {
    const std::vector<std::size_t> sizes{300, 3000, 15000, 45000};
    std::cout << "packet_count,sequential_us,parallel_us,total\n";

    for (const auto size : sizes) {
        auto server = createServer(size);
        network_server::model::PacketPercentages sequentialResult;
        network_server::model::PacketPercentages parallelResult;

        const auto sequentialTime = measureMicroseconds([&server, &sequentialResult]() {
            sequentialResult = server.percentagesSequential();
        });
        const auto parallelTime = measureMicroseconds([&server, &parallelResult]() {
            parallelResult = server.percentagesParallel();
        });

        std::cout << size << ',' << sequentialTime << ',' << parallelTime << ','
                  << parallelResult.total << '\n';

        if (sequentialResult.total != parallelResult.total) {
            return 2;
        }
    }
    return 0;
}
