#ifndef CLIENT_APP_SETTINGS_H
#define CLIENT_APP_SETTINGS_H

#include <QSettings>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <string>
#include "logger.h"

/**
 * Application settings manager that combines QSettings with command-line options
 */
class AppSettings {
public:
    static AppSettings& getInstance() {
        static AppSettings instance;
        return instance;
    }
    
    void parseCommandLine(const QCoreApplication& app) {
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
    
    // Server settings
    std::string getServerAddress() const {
        return _settings.value("server/address", "localhost").toString().toStdString();
    }
    
    int getServerPort() const {
        return _settings.value("server/port", 8080).toInt();
    }
    
    // User settings
    std::string getUsername() const {
        return _settings.value("user/username", "").toString().toStdString();
    }
    
    std::string getPassword() const {
        return _settings.value("user/password", "").toString().toStdString();
    }
    
    bool isAutoLoginEnabled() const {
        return _settings.value("user/autoLogin", false).toBool();
    }
    
    // Logging settings
    bool isVerboseLogging() const {
        return _settings.value("logging/verbose", false).toBool();
    }
    
    // UI settings
    bool rememberWindowSize() const {
        return _settings.value("ui/rememberSize", true).toBool();
    }
    
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
    bool isInitialized() const {
        return _initialized;
    }
    
private:
    AppSettings() : _settings("DeskPP", "Client"), _initialized(false) {
        // Constructor is private for singleton
    }
    
    ~AppSettings() = default;
    
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;
    
    QSettings _settings;
    bool _initialized;
};

#endif // CLIENT_APP_SETTINGS_H