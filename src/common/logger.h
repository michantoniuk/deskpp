#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>

inline void initLogger(const std::string &appName, bool verbose = false) {
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>(appName, console);
    logger->set_level(verbose ? spdlog::level::debug : spdlog::level::info);
    logger->set_pattern("[%H:%M:%S %z] [%^%l%$] %v", spdlog::pattern_time_type::utc);
    spdlog::set_default_logger(logger);
}

#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARNING(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

#endif // LOGGER_H
