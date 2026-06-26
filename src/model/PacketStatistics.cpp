#include "network_server/model/PacketStatistics.hpp"

#include <array>
#include <future>

namespace network_server::model {

namespace {

std::size_t countByType(const TransmissionTable& table, PacketType type) {
    std::size_t count = 0;
    for (const auto& entry : table) {
        if (entry.value->type() == type) {
            ++count;
        }
    }
    return count;
}

PacketPercentages makePercentages(PacketCounts counts) {
    const auto total = counts.mail + counts.file + counts.hypertext;
    if (total == 0) {
        return {};
    }

    return PacketPercentages{
        static_cast<double>(counts.mail) * 100.0 / static_cast<double>(total),
        static_cast<double>(counts.file) * 100.0 / static_cast<double>(total),
        static_cast<double>(counts.hypertext) * 100.0 / static_cast<double>(total),
        total};
}

}  // namespace

PacketPercentages PacketStatistics::calculateSequential(const TransmissionTable& table) {
    PacketCounts counts;
    for (const auto& entry : table) {
        switch (entry.value->type()) {
            case PacketType::Mail:
                ++counts.mail;
                break;
            case PacketType::File:
                ++counts.file;
                break;
            case PacketType::Hypertext:
                ++counts.hypertext;
                break;
        }
    }
    return makePercentages(counts);
}

PacketPercentages PacketStatistics::calculateParallel(const TransmissionTable& table) {
    auto mailCount = std::async(std::launch::async, countByType, std::cref(table), PacketType::Mail);
    auto fileCount = std::async(std::launch::async, countByType, std::cref(table), PacketType::File);
    auto hypertextCount =
        std::async(std::launch::async, countByType, std::cref(table), PacketType::Hypertext);

    return makePercentages(
        PacketCounts{mailCount.get(), fileCount.get(), hypertextCount.get()});
}

}  // namespace network_server::model
