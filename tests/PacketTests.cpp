#include "network_server/model/FilePacket.hpp"
#include "network_server/model/HypertextPacket.hpp"
#include "network_server/model/MailPacket.hpp"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

using namespace network_server::model;

TEST_CASE("Mail packet validates user name length") {
    REQUIRE_THROWS_AS(
        MailPacket("1.1.1.1", "2.2.2.2", "this-name-is-way-too-long", MessageDescriptor{"hello"}),
        std::invalid_argument);
}

TEST_CASE("Mail packet converts to file packet") {
    MailPacket mail{"1.1.1.1", "2.2.2.2", "dashima", MessageDescriptor{"hello"}};

    const auto file = mail.toFilePacket(CodeType::BIN, InfoType::Control);

    REQUIRE(file->type() == PacketType::File);
    REQUIRE(file->senderAddress() == "1.1.1.1");
    REQUIRE(file->recipientAddress() == "2.2.2.2");
    REQUIRE(file->codeType() == CodeType::BIN);
    REQUIRE(file->infoType() == InfoType::Control);
    REQUIRE(file->message().content() == "hello");
}

TEST_CASE("File packet converts to hypertext packet with links") {
    FilePacket file{"1.1.1.1", "2.2.2.2", CodeType::ASCII, InfoType::Data, MessageDescriptor{"data"}};

    auto hypertext = file.toHypertextPacket({LinkDescriptor{ProtocolType::HTTP, "www.mephi.ru"}});

    REQUIRE(hypertext->type() == PacketType::Hypertext);
    REQUIRE(hypertext->linkCount() == 1);
    REQUIRE(hypertext->links().front().protocol() == ProtocolType::HTTP);
}
