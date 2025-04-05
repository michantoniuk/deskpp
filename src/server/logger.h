// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h" // Dodajemy brakujący nagłówek

// Makra dla kompatybilności z istniejącym kodem
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARNING(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

// Inicjalizacja loggera
inline void initLogger(const std::string& filename) {
    // Utwórz logger zapisujący do pliku
    auto file_logger = spdlog::basic_logger_mt("biurkopp", filename);
    spdlog::set_default_logger(file_logger);

    // Ustaw format logów
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%l] %v");

    // Ustaw poziom logowania
    spdlog::set_level(spdlog::level::debug);

    // Włącz flushing po każdym logu (opcjonalnie)
    spdlog::flush_on(spdlog::level::info);
}

#endif // LOGGER_H