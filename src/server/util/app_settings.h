#ifndef SERVER_APP_SETTINGS_H
#define SERVER_APP_SETTINGS_H

#include <string>
#include <iostream>
#include <filesystem>
#include "logger.h"

/**
 * Application settings manager for the server
 */
class AppSettings {
public:
    static AppSettings& getInstance() {
        static AppSettings instance;
        return instance;
    }

    void parseCommandLine(int argc, char* argv[]) {
        for (int i = 1; i < argc; i++) {
            if ((strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) && i + 1 < argc) {
                _port = std::stoi(argv[i + 1]);
                i++;
            } else if ((strcmp(argv[i], "--database") == 0 || strcmp(argv[i], "-db") == 0) && i + 1 < argc) {
                _dbPath = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
                _verbose = true;
            } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                showHelp();
                exit(0);
            }
        }

        _initialized = true;
        
        LOG_INFO("Settings initialized from command line");
    }

    // Server settings
    int getPort() const {
        return _port;
    }

    std::string getDatabasePath() const {
        return _dbPath;
    }

    bool isVerboseLogging() const {
        return _verbose;
    }

    // State management
    bool isInitialized() const {
        return _initialized;
    }

private:
    AppSettings() : _port(8080), _dbPath("deskpp.sqlite"), _verbose(false), _initialized(false) {
        // Constructor is private for singleton
    }

    ~AppSettings() = default;

    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;

    void showHelp() {
        std::cout << "DeskPP Server\n"
                << "Usage:\n"
                << "  --port, -p PORT       Set server port (default: 8080)\n"
                << "  --database, -db PATH  Set database path (default: deskpp.sqlite)\n"
                << "  --verbose, -v         Enable verbose logging\n"
                << "  --help, -h            Show this help message\n";
    }

    int _port;
    std::string _dbPath;
    bool _verbose;
    bool _initialized;
};

#endif // SERVER_APP_SETTINGS_H