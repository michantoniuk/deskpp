#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QSettings>
#include <QCoreApplication>
#include <QCommandLineParser>
#include "logger.h"

/**
 * @class AppSettings
 * @brief Klasa zarządzająca ustawieniami aplikacji.
 *
 * Implementuje wzorzec Singleton i zapewnia dostęp do ustawień aplikacji.
 * Obsługuje parametry linii poleceń i stałe konfiguracyjne.
 */
class AppSettings {
public:
    /**
     * @brief Pobiera instancję klasy (implementacja wzorca Singleton)
     * @return Referencja do jednej instancji klasy
     */
    static AppSettings &getInstance() {
        static AppSettings instance;
        return instance;
    }

    /**
     * @brief Parsuje argumenty linii poleceń dla klienta
     * @param app Referencja do obiektu aplikacji Qt
     */
    void parseCommandLine(const QCoreApplication &app) {
        QCommandLineParser parser;
        parser.setApplicationDescription("DeskPP Client");
        parser.addHelpOption();

        QCommandLineOption serverOption(QStringList() << "s" << "server", "Adres serwera", "address", "localhost");
        QCommandLineOption portOption(QStringList() << "p" << "port", "Port serwera", "port", "8080");
        QCommandLineOption verboseOption(QStringList() << "v" << "verbose", "Włącz szczegółowe logowanie");

        parser.addOption(serverOption);
        parser.addOption(portOption);
        parser.addOption(verboseOption);
        parser.process(app);

        if (parser.isSet(serverOption)) _settings.setValue("server/address", parser.value(serverOption));
        if (parser.isSet(portOption)) _settings.setValue("server/port", parser.value(portOption).toInt());
        if (parser.isSet(verboseOption)) _settings.setValue("logging/verbose", true);

        _initialized = true;
    }

    /**
     * @brief Parsuje argumenty linii poleceń dla serwera
     * @param argc Liczba argumentów
     * @param argv Tablica argumentów
     */
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

    /**
     * @brief Pobiera adres serwera
     * @return Adres serwera
     */
    std::string getServerAddress() const {
        return _settings.value("server/address", "localhost").toString().toStdString();
    }

    /**
     * @brief Pobiera port serwera (konfiguracja klienta)
     * @return Port serwera
     */
    int getServerPort() const { return _settings.value("server/port", 8080).toInt(); }

    /**
     * @brief Pobiera port serwera (konfiguracja serwera)
     * @return Port serwera
     */
    int getPort() const { return _port; }

    /**
     * @brief Pobiera ścieżkę do pliku bazy danych
     * @return Ścieżka do pliku bazy danych
     */
    std::string getDatabasePath() const { return _dbPath; }

    /**
     * @brief Sprawdza czy włączone jest szczegółowe logowanie
     * @return Czy włączone jest szczegółowe logowanie
     */
    bool isVerboseLogging() const { return _settings.value("logging/verbose", false).toBool() || _verbose; }

    /**
     * @brief Sprawdza czy ustawienia zostały zainicjalizowane
     * @return Czy ustawienia zostały zainicjalizowane
     */
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
