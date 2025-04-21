#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QSettings>
#include <QCoreApplication>
#include <QCommandLineParser>
#include "logger.h"

class AppSettings {
public:
    static AppSettings &getInstance() {
        static AppSettings instance;
        return instance;
    }

    // Client settings init
    void parseCommandLine(const QCoreApplication &app) {
        QCommandLineParser parser;
        parser.setApplicationDescription("DeskPP Client");
        parser.addHelpOption();

        QCommandLineOption serverOption(QStringList() << "s" << "server", "Server address", "address", "localhost");
        QCommandLineOption portOption(QStringList() << "p" << "port", "Server port", "port", "8080");
        QCommandLineOption verboseOption(QStringList() << "v" << "verbose", "Enable verbose logging");

        parser.addOption(serverOption);
        parser.addOption(portOption);
        parser.addOption(verboseOption);
        parser.process(app);

        if (parser.isSet(serverOption)) _settings.setValue("server/address", parser.value(serverOption));
        if (parser.isSet(portOption)) _settings.setValue("server/port", parser.value(portOption).toInt());
        if (parser.isSet(verboseOption)) _settings.setValue("logging/verbose", true);

        _initialized = true;
        LOG_INFO("Settings initialized");
    }

    // Server settings init
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
            }
        }
        _initialized = true;
    }

    // Getters
    std::string getServerAddress() const {
        return _settings.value("server/address", "localhost").toString().toStdString();
    }

    int getServerPort() const { return _settings.value("server/port", 8080).toInt(); }
    int getPort() const { return _port; }
    std::string getDatabasePath() const { return _dbPath; }
    bool isVerboseLogging() const { return _settings.value("logging/verbose", false).toBool() || _verbose; }
    bool isInitialized() const { return _initialized; }

private:
    AppSettings() : _settings("DeskPP", "Application"), _initialized(false), _port(8080),
                    _dbPath("deskpp.sqlite"), _verbose(false) {
    }

    AppSettings(const AppSettings &) = delete;

    AppSettings &operator=(const AppSettings &) = delete;

    QSettings _settings;
    bool _initialized;
    int _port;
    std::string _dbPath;
    bool _verbose;
};

#endif
