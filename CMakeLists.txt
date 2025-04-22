cmake_minimum_required(VERSION 3.14)
project(DeskPP LANGUAGES CXX)

# Użyj C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Znajdź pakiety Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Zależności
include(FetchContent)

# Biblioteka JSON dla komunikacji API
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# spdlog dla logowania
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.10.0
)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "")
FetchContent_MakeAvailable(spdlog)

# SQLiteCpp dla operacji bazodanowych
FetchContent_Declare(sqlitecpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
        GIT_TAG 3.3.1
)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "")
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "")
set(SQLITECPP_INTERNAL_SQLITE OFF CACHE BOOL "")
FetchContent_MakeAvailable(sqlitecpp)

# Znajdź SQLite3 dla serwera
find_package(SQLite3 REQUIRED)

# Crow dla REST API
FetchContent_Declare(crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(crow)

# Wspólne katalogi include
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${json_SOURCE_DIR}/include
        ${sqlitecpp_SOURCE_DIR}/include
        ${SQLite3_INCLUDE_DIRS}
)

# Wspólne źródła
set(COMMON_SOURCES
        src/common/logger.h
        src/common/app_settings.h
        src/common/model/model.h
        src/common/model/entity.h
        src/common/model/user.h
        src/common/model/user.cpp
        src/common/model/building.h
        src/common/model/building.cpp
        src/common/model/desk.h
        src/common/model/desk.cpp
        src/common/model/booking.h
        src/common/model/booking.cpp
)

# Źródła klienta
set(CLIENT_SOURCES
        ${COMMON_SOURCES}
        src/client/ui/booking_view.h
        src/client/ui/booking_view.cpp
        src/client/ui/booking_dialog.h
        src/client/ui/booking_dialog.cpp
        src/client/ui/login_dialog.h
        src/client/ui/login_dialog.cpp
        src/client/net/api_client.h
        src/client/net/api_client.cpp
        src/client/main.cpp
)

# Źródła serwera
set(SERVER_SOURCES
        ${COMMON_SOURCES}
        src/server/repository/repository.h
        src/server/repository/sqlite_repository.h
        src/server/repository/user_repository.h
        src/server/repository/user_repository.cpp
        src/server/repository/desk_repository.h
        src/server/repository/desk_repository.cpp
        src/server/repository/booking_repository.h
        src/server/repository/booking_repository.cpp
        src/server/repository/building_repository.h
        src/server/repository/building_repository.cpp
        src/server/service/service.h
        src/server/service/user_service.h
        src/server/service/user_service.cpp
        src/server/service/booking_service.h
        src/server/service/booking_service.cpp
        src/server/api/controller/controller.h
        src/server/api/controller/booking_controller.h
        src/server/api/controller/booking_controller.cpp
        src/server/api/controller/user_controller.h
        src/server/api/controller/user_controller.cpp
        src/server/api/routes.h
        src/server/api/routes.cpp
        src/server/main.cpp
)

# Buduj klienta
add_executable(deskpp_client ${CLIENT_SOURCES})
target_link_libraries(deskpp_client PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

# Buduj serwer
add_executable(deskpp_server ${SERVER_SOURCES})
target_link_libraries(deskpp_server PRIVATE
        nlohmann_json::nlohmann_json
        SQLite::SQLite3
        SQLiteCpp
        spdlog::spdlog
        Crow::Crow
        Qt6::Core
)