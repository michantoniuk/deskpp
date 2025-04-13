cmake_minimum_required(VERSION 3.14)
project(DeskPP LANGUAGES CXX)

# Set policy for DOWNLOAD_EXTRACT_TIMESTAMP to avoid warning
if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif ()

# Compilation settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies for both client and server
include(FetchContent)

# nlohmann/json (both client and server)
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# spdlog (both client and server)
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(spdlog)

# Client-specific dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Boost (client-only)
FetchContent_Declare(boost_beast
        GIT_REPOSITORY https://github.com/boostorg/beast.git
        GIT_TAG boost-1.79.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(boost_asio
        GIT_REPOSITORY https://github.com/boostorg/asio.git
        GIT_TAG boost-1.79.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(boost_beast boost_asio)

# Server-specific dependencies
# Fix SQLite3 dependency - use find_package properly
find_package(SQLite3 REQUIRED)
if (NOT SQLite3_FOUND)
    message(FATAL_ERROR "SQLite3 library not found!")
endif ()

FetchContent_Declare(crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.0+5
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(crow)

# Include paths
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${boost_beast_SOURCE_DIR}/include
        ${boost_asio_SOURCE_DIR}/include
        ${crow_SOURCE_DIR}/include
)

# CLIENT FILES
set(CLIENT_MODEL_FILES
        src/client/model/booking.h
        src/client/model/booking.cpp
        src/client/model/desk.h
        src/client/model/desk.cpp
)

set(CLIENT_UI_FILES
        src/client/ui/booking_view.h
        src/client/ui/booking_view.cpp
        src/client/ui/booking_dialog.h
        src/client/ui/booking_dialog.cpp
)

set(CLIENT_NET_FILES
        src/client/net/client_communication.h
        src/client/net/client_communication.cpp
)

set(CLIENT_UTIL_FILES
        src/client/util/logger.h
        src/client/util/cmd_args.h
        src/client/util/client_error.h
)

set(CLIENT_SOURCES
        src/client/main.cpp
        ${CLIENT_MODEL_FILES}
        ${CLIENT_UI_FILES}
        ${CLIENT_NET_FILES}
        ${CLIENT_UTIL_FILES}
)

# SERVER FILES
set(SERVER_MODEL_FILES
        src/server/model/booking.h
        src/server/model/booking.cpp
        src/server/model/desk.h
        src/server/model/desk.cpp
        src/server/model/building.h
        src/server/model/building.cpp
)

set(SERVER_DB_FILES
        src/server/db/database.h
        src/server/db/database.cpp
)

set(SERVER_REPOSITORY_FILES
        src/server/repository/booking_repository.h
        src/server/repository/booking_repository.cpp
        src/server/repository/desk_repository.h
        src/server/repository/desk_repository.cpp
        src/server/repository/building_repository.h
        src/server/repository/building_repository.cpp
)

set(SERVER_API_FILES
        src/server/api/booking_controller.h
        src/server/api/booking_controller.cpp
        src/server/api/routing.h
        src/server/api/routing.cpp
)

set(SERVER_SERVICE_FILES
        src/server/service/booking_service.h
        src/server/service/booking_service.cpp
)

set(SERVER_UTIL_FILES
        src/server/util/logger.h
        src/server/util/cmd_args.h
        src/server/util/date_helper.h
        src/server/util/date_helper.cpp
)

set(SERVER_SOURCES
        src/server/main.cpp
        ${SERVER_MODEL_FILES}
        ${SERVER_DB_FILES}
        ${SERVER_REPOSITORY_FILES}
        ${SERVER_API_FILES}
        ${SERVER_SERVICE_FILES}
        ${SERVER_UTIL_FILES}
)

# Build targets
add_executable(deskpp_client ${CLIENT_SOURCES})
target_link_libraries(deskpp_client PRIVATE
        Qt6::Core
        Qt6::Widgets
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

add_executable(deskpp_server ${SERVER_SOURCES})
target_link_libraries(deskpp_server PRIVATE
        nlohmann_json::nlohmann_json
        ${SQLite3_LIBRARIES}  # Use the correct variable name for SQLite3
        spdlog::spdlog
)

# Add include directories for SQLite3
target_include_directories(deskpp_server PRIVATE ${SQLite3_INCLUDE_DIRS})