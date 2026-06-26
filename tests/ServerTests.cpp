#include "network_server/model/FilePacket.hpp"
#include "network_server/model/HypertextPacket.hpp"
#include "network_server/model/MailPacket.hpp"
#include "network_server/model/ServerDescriptor.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

using namespace network_server::model;

TEST_CASE("Server selects packet by required priority") {
    ServerDescriptor server{"test.local", "127.0.0.1"};
    server.addPacket(std::make_shared<MailPacket>("1.1.1.1", "9.9.9.9", "user", MessageDescriptor{"mail"}));
    server.addPacket(std::make_shared<FilePacket>(
        "2.2.2.2",
        "9.9.9.9",
        CodeType::ASCII,
        InfoType::Data,
        MessageDescriptor{"file"}));
    server.addPacket(std::make_shared<HypertextPacket>(
        "3.3.3.3",
        "9.9.9.9",
        CodeType::BIN,
        InfoType::Control,
        MessageDescriptor{"html"},
        std::vector<LinkDescriptor>{}));

    const auto selected = server.selectByRecipientPriority("9.9.9.9");

    REQUIRE(selected != nullptr);
    REQUIRE(selected->type() == PacketType::Hypertext);
    REQUIRE(server.findPacket("9.9.9.9", PacketType::Hypertext) == nullptr);
    REQUIRE(server.findPacket("9.9.9.9", PacketType::File) != nullptr);
}

TEST_CASE("Server calculates equal sequential and parallel percentages") {
    ServerDescriptor server{"test.local", "127.0.0.1"};
    server.addPacket(std::make_shared<MailPacket>("1.1.1.1", "1.1.1.2", "user", MessageDescriptor{"mail"}));
    server.addPacket(std::make_shared<FilePacket>(
        "1.1.1.3",
        "1.1.1.4",
        CodeType::ASCII,
        InfoType::Data,
        MessageDescriptor{"file"}));
    server.addPacket(std::make_shared<HypertextPacket>(
        "1.1.1.5",
        "1.1.1.6",
        CodeType::BIN,
        InfoType::Control,
        MessageDescriptor{"html"},
        std::vector<LinkDescriptor>{}));

    const auto sequential = server.percentagesSequential();
    const auto parallel = server.percentagesParallel();

    REQUIRE(sequential.total == 3);
    REQUIRE(parallel.total == 3);
    REQUIRE(sequential.mail == parallel.mail);
    REQUIRE(sequential.file == parallel.file);
    REQUIRE(sequential.hypertext == parallel.hypertext);
}
