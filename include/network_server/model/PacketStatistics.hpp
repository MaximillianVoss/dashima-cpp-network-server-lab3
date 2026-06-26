#pragma once

#include "network_server/model/TransmissionTable.hpp"

#include <cstddef>

namespace network_server::model {

/**
 * @brief Packet counts grouped by type.
 */
struct PacketCounts {
    std::size_t mail = 0;
    std::size_t file = 0;
    std::size_t hypertext = 0;
};

/**
 * @brief Packet percentages grouped by type.
 */
struct PacketPercentages {
    double mail = 0.0;
    double file = 0.0;
    double hypertext = 0.0;
    std::size_t total = 0;
};

/**
 * @brief Calculates packet statistics using transmission table iterators.
 */
class PacketStatistics {
public:
    /**
     * @brief Calculates percentages in one thread.
     */
    static PacketPercentages calculateSequential(const TransmissionTable& table);

    /**
     * @brief Calculates percentages in three threads, one thread per packet type.
     */
    static PacketPercentages calculateParallel(const TransmissionTable& table);
};

}  // namespace network_server::model
