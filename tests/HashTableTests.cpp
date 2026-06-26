#include "network_server/container/HashTable.hpp"

#include <catch2/catch_test_macros.hpp>
#include <string>

using network_server::container::HashTable;

TEST_CASE("HashTable inserts finds replaces and erases values") {
    HashTable<std::string, int> table{3};

    REQUIRE(table.empty());
    REQUIRE(table.insert("alpha", 1));
    REQUIRE(table.insert("beta", 2));
    REQUIRE_FALSE(table.insert("alpha", 10));

    REQUIRE(table.size() == 2);
    REQUIRE(table.find("alpha") != nullptr);
    REQUIRE(*table.find("alpha") == 10);
    REQUIRE(table.contains("beta"));

    REQUIRE(table.erase("beta"));
    REQUIRE_FALSE(table.contains("beta"));
    REQUIRE_FALSE(table.erase("missing"));
}

TEST_CASE("HashTable iterator visits all entries") {
    HashTable<std::string, int> table{3};
    table.insert("one", 1);
    table.insert("two", 2);
    table.insert("three", 3);

    int sum = 0;
    for (const auto& entry : table) {
        sum += entry.value;
    }

    REQUIRE(sum == 6);
}
