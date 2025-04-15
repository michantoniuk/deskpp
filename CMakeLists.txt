cmake_minimum_required(VERSION 3.14)
project(DeskPP LANGUAGES CXX)

# Suppress warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE INTERNAL "Suppress developer warnings")
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "Disable deprecation warnings")
set(CMAKE_POLICY_DEFAULT_CMP0167 OLD)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Dependencies
include(FetchContent)

# JSON library
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# spdlog for logging
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.10.0  # Use a version compatible with fmt 10.0.0
)
# Tell spdlog to use its bundled fmt library
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "")
FetchContent_MakeAvailable(spdlog)

# SQLiteCpp (replaces raw SQLite3 + custom database layer)
FetchContent_Declare(sqlitecpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
        GIT_TAG 3.3.1
)
# SQLiteCpp options
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "")
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "")
set(SQLITECPP_INTERNAL_SQLITE OFF CACHE BOOL "")  # Use system sqlite
FetchContent_MakeAvailable(sqlitecpp)

# Find SQLite3 for server
find_package(SQLite3 REQUIRED)

# Crow for REST API - using version 1.0+5 which doesn't require ASIO
FetchContent_Declare(crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(crow)

# Common include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${json_SOURCE_DIR}/include
        ${sqlitecpp_SOURCE_DIR}/include
        ${SQLite3_INCLUDE_DIRS}
)

# Client sources
set(CLIENT_SOURCES
        src/client/model/models.h
        src/client/model/models.cpp
        src/client/ui/booking_view.h
        src/client/ui/booking_view.cpp
        src/client/ui/booking_dialog.h
        src/client/ui/booking_dialog.cpp
        src/client/ui/login_dialog.h
        src/client/ui/login_dialog.cpp
        src/client/net/api_client.h
        src/client/net/api_client.cpp
        src/client/util/logger.h
        src/client/util/app_settings.h
        src/client/main.cpp
)

# Server sources
set(SERVER_SOURCES
        src/server/model/models.h
        src/server/model/models.cpp
        src/server/api/controllers.h
        src/server/api/controllers.cpp
        src/server/api/api_routes.cpp
        src/server/service/services.h
        src/server/service/services.cpp
        src/server/db/database.h
        src/server/db/database.cpp
        src/server/util/logger.h
        src/server/util/app_settings.h
        src/server/main.cpp
)

# Build client
add_executable(deskpp_client ${CLIENT_SOURCES})
target_link_libraries(deskpp_client PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

# Build server
add_executable(deskpp_server ${SERVER_SOURCES})
target_link_libraries(deskpp_server PRIVATE
        nlohmann_json::nlohmann_json
        SQLite::SQLite3
        SQLiteCpp
        spdlog::spdlog
        Crow::Crow
)