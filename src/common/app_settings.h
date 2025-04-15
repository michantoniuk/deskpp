#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <string>
#include <QSettings>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QSize>  // Add missing QSize header
#include <iostream>
#include "logger.h"

/**
 * Application settings manager as a singleton
 */
class AppSettings {
public:
    static AppSettings &getInstance() {
        static AppSettings instance;
        return instance;
    }

    // Client settings initialization
    void parseCommandLine(const QCoreApplication &app) {
        QCommandLineParser parser;
        parser.setApplicationDescription("DeskPP Client");
        parser.addHelpOption();

        // Server options
        QCommandLineOption serverOption(QStringList() << "s" << "server",
                                        "Server address", "address", "localhost");
        QCommandLineOption portOption(QStringList() << "p" << "port",
                                      "Server port", "port", "8080");

        // User options
        QCommandLineOption usernameOption(QStringList() << "u" << "username",
                                          "Username for auto-login", "username");
        QCommandLineOption passwordOption(QStringList() << "pw" << "password",
                                          "Password for auto-login", "password");
        QCommandLineOption verboseOption(QStringList() << "v" << "verbose",
                                         "Enable verbose logging");

        parser.addOption(serverOption);
        parser.addOption(portOption);
        parser.addOption(usernameOption);
        parser.addOption(passwordOption);
        parser.addOption(verboseOption);

        parser.process(app);

        // Apply command-line options, overriding stored settings
        if (parser.isSet(serverOption)) {
            _settings.setValue("server/address", parser.value(serverOption));
        }

        if (parser.isSet(portOption)) {
            _settings.setValue("server/port", parser.value(portOption).toInt());
        }

        if (parser.isSet(usernameOption)) {
            _settings.setValue("user/username", parser.value(usernameOption));
            _settings.setValue("user/autoLogin", true);
        }

        if (parser.isSet(passwordOption)) {
            _settings.setValue("user/password", parser.value(passwordOption));
        }

        if (parser.isSet(verboseOption)) {
            _settings.setValue("logging/verbose", true);
        }

        _initialized = true;

        LOG_INFO("Settings initialized from command line");
    }

    // Server settings initialization
    void parseCommandLine(int argc, char *argv[]) {
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
                std::cout << "DeskPP Server\n"
                        << "Usage:\n"
                        << "  --port, -p PORT       Set server port (default: 8080)\n"
                        << "  --database, -db PATH  Set database path (default: deskpp.sqlite)\n"
                        << "  --verbose, -v         Enable verbose logging\n"
                        << "  --help, -h            Show this help message\n";
                exit(0);
            }
        }

        _initialized = true;

        LOG_INFO("Settings initialized from command line");
    }

    // Server settings
    std::string getServerAddress() const {
        return _settings.value("server/address", "localhost").toString().toStdString();
    }

    int getServerPort() const { return _settings.value("server/port", 8080).toInt(); }

    // Add server-specific methods
    int getPort() const { return _port; }
    std::string getDatabasePath() const { return _dbPath; }

    // User settings
    std::string getUsername() const { return _settings.value("user/username", "").toString().toStdString(); }
    std::string getPassword() const { return _settings.value("user/password", "").toString().toStdString(); }
    bool isAutoLoginEnabled() const { return _settings.value("user/autoLogin", false).toBool(); }

    // Logging settings
    bool isVerboseLogging() const { return _settings.value("logging/verbose", false).toBool() || _verbose; }

    // UI settings
    bool rememberWindowSize() const { return _settings.value("ui/rememberSize", true).toBool(); }

    void saveWindowSize(int width, int height) {
        _settings.setValue("ui/width", width);
        _settings.setValue("ui/height", height);
    }

    QSize getWindowSize() const {
        int width = _settings.value("ui/width", 800).toInt();
        int height = _settings.value("ui/height", 600).toInt();
        return QSize(width, height);
    }

    // State management
    bool isInitialized() const { return _initialized; }

private:
    AppSettings() : _settings("DeskPP", "Application"),
                    _initialized(false),
                    _port(8080),
                    _dbPath("deskpp.sqlite"),
                    _verbose(false) {
    }

    ~AppSettings() = default;

    AppSettings(const AppSettings &) = delete;

    AppSettings &operator=(const AppSettings &) = delete;

    QSettings _settings;
    bool _initialized;

    // Server specific settings
    int _port;
    std::string _dbPath;
    bool _verbose;
};

#endif // APP_SETTINGS_H
