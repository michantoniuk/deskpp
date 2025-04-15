#ifndef CLIENT_LOGGER_H
#define CLIENT_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>

// Initialize the logger
inline void initLogger(bool verbose = false) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("deskpp_client", console_sink);
    logger->set_level(verbose ? spdlog::level::debug : spdlog::level::info);
    logger->set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::set_default_logger(logger);
}

// Simple log macros using spdlog
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARNING(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

#endif // CLIENT_LOGGER_H
