cmake_minimum_required(VERSION 3.20)

project(network_server_lab3 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

add_library(network_server_model
    src/model/FilePacket.cpp
    src/model/HypertextPacket.cpp
    src/model/LinkDescriptor.cpp
    src/model/MailPacket.cpp
    src/model/MessageDescriptor.cpp
    src/model/Packet.cpp
    src/model/PacketStatistics.cpp
    src/model/PacketTypes.cpp
    src/model/ServerDescriptor.cpp
    src/model/TransmissionTable.cpp
)

target_include_directories(network_server_model
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(network_server_model PRIVATE /W4 /permissive-)
else()
    target_compile_options(network_server_model PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(network_server_ui
    src/controller/ServerController.cpp
    src/view/ConsoleView.cpp
)

target_link_libraries(network_server_ui
    PUBLIC
        network_server_model
)

target_include_directories(network_server_ui
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(network_server_ui PRIVATE /W4 /permissive-)
else()
    target_compile_options(network_server_ui PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(network_server_dialog app/main.cpp)
target_link_libraries(network_server_dialog PRIVATE network_server_ui)

add_executable(network_server_benchmark tools/benchmark.cpp)
target_link_libraries(network_server_benchmark PRIVATE network_server_model)

if(BUILD_TESTING)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(network_server_tests
        tests/HashTableTests.cpp
        tests/PacketTests.cpp
        tests/ServerTests.cpp
    )
    target_link_libraries(network_server_tests
        PRIVATE
            Catch2::Catch2WithMain
            network_server_model
    )

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras ${Catch2_SOURCE_DIR}/extras)
    include(Catch)
    catch_discover_tests(network_server_tests DISCOVERY_MODE PRE_TEST)
endif()
