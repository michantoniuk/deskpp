cmake_minimum_required(VERSION 3.14)
project(BiurkoPP VERSION 0.1.0 LANGUAGES CXX)

# Ustawienia C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dołącz katalog src do ścieżki include
include_directories(${CMAKE_SOURCE_DIR})

# Znajdź pakiet Qt6
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Automatyczna obsługa plików Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Pliki źródłowe
set(SOURCES
        src/main.cpp
        src/biurko.cpp
        src/widok_rezerwacji.cpp
        src/dialog_rezerwacji.cpp
)

# Pliki nagłówkowe
set(HEADERS
        src/biurko.h
        src/widok_rezerwacji.h
)

# Utwórz plik wykonywalny
add_executable(biurkopp
        ${SOURCES}
        ${HEADERS}
)

# Linkowanie z Qt6
target_link_libraries(biurkopp PRIVATE Qt6::Core Qt6::Widgets)