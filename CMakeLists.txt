cmake_minimum_required(VERSION 3.14)
project(BiurkoPP VERSION 0.1.0 LANGUAGES CXX)

# Ustawienia C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dołącz katalog src do ścieżki include
include_directories(${CMAKE_SOURCE_DIR})

# Znajdź Boost - potrzebny dla Crow i komunikacji klienta
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

# Znajdź nlohmann/json (zakładamy, że jest zainstalowany systemowo lub przez vcpkg)
find_package(nlohmann_json CONFIG REQUIRED)

# Znajdź spdlog
find_package(spdlog REQUIRED)

# Znajdź Crow - można zainstalować przez vcpkg lub pobrać ręcznie
find_package(Crow CONFIG QUIET)
if (NOT Crow_FOUND)
    # Jeśli Crow nie jest zainstalowany systemowo, pobierzmy go z GitHuba
    include(FetchContent)
    FetchContent_Declare(
            Crow
            GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
            GIT_TAG v1.0
    )
    FetchContent_MakeAvailable(Crow)
endif()

# Pliki źródłowe dla klienta
set(CLIENT_SOURCES
        src/client/main_client.cpp
        src/client/komunikacja_klienta.cpp
        src/client/widok_rezerwacji.cpp
        src/client/dialog_rezerwacji.cpp
        src/common/biurko.cpp
)

# Pliki nagłówkowe dla klienta
set(CLIENT_HEADERS
        src/client/komunikacja_klienta.h
        src/client/widok_rezerwacji.h
        src/client/dialog_rezerwacji.h
        src/common/biurko.h
)

# Pliki źródłowe dla serwera (mocno uproszczone!)
set(SERVER_SOURCES
        src/server/main_server.cpp
        src/server/kontroler_serwera.cpp
)

# Pliki nagłówkowe dla serwera (mocno uproszczone!)
set(SERVER_HEADERS
        src/server/kontroler_serwera.h
        src/server/logger.h
)

# Znajdź pakiet Qt dla klienta
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_VERSION 5)
else()
    set(QT_VERSION 6)
endif()

# Automatyczna obsługa plików Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Utwórz plik wykonywalny dla klienta
add_executable(biurkopp_client ${CLIENT_SOURCES} ${CLIENT_HEADERS})

# Utwórz plik wykonywalny dla serwera
add_executable(biurkopp_server ${SERVER_SOURCES} ${SERVER_HEADERS})

# Linkowanie z Qt (wersja zależna od tego, co znaleziono)
if(QT_VERSION EQUAL 6)
    target_link_libraries(biurkopp_client PRIVATE
            Qt6::Core
            Qt6::Widgets
    )
else()
    target_link_libraries(biurkopp_client PRIVATE
            Qt5::Core
            Qt5::Widgets
    )
endif()

# Linkowanie bibliotek dla serwera (uproszczone)
target_link_libraries(biurkopp_server PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        Crow::Crow
        pthread
)

# Linkowanie bibliotek dla klienta
target_link_libraries(biurkopp_client PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json
)

# Dodaj komunikat o wersji Qt
message(STATUS "Building with Qt${QT_VERSION}")