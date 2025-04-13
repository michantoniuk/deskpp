#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <iostream>

#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARNING(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

inline void initLogger(const spdlog::level::level_enum level = spdlog::level::info,
                      bool logToFile = false,
                      const std::string& filename = "deskpp_server.log") {
    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        if (logToFile) {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
            std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
            auto logger = std::make_shared<spdlog::logger>("deskpp_server", sinks.begin(), sinks.end());
            
            logger->set_level(level);
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
            spdlog::set_default_logger(logger);
        } else {
            auto logger = std::make_shared<spdlog::logger>("deskpp_server", console_sink);
            logger->set_level(level);
            logger->set_pattern("[%H:%M:%S] [%^%l%$] %v");
            spdlog::set_default_logger(logger);
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
}

#endif // LOGGER_H