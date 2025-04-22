#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QNetworkAccessManager>
#include <QObject>
#include <optional>
#include "common/model/model.h"

/**
 * @class ApiClient
 * @brief Klasa obsługująca komunikację z serwerem API.
 *
 * ApiClient zapewnia funkcje do komunikacji z serwerem REST API,
 * umożliwiając pobieranie danych o budynkach, biurkach, zarządzanie rezerwacjami
 * oraz operacje użytkownika takie jak logowanie i rejestracja.
 */
class ApiClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Konstruktor domyślny
     * @param parent Obiekt rodzica (opcjonalny)
     */
    ApiClient(QObject *parent = nullptr);

    /**
     * @brief Konstruktor z parametrami adresu serwera
     * @param serverAddress Adres serwera
     * @param port Port serwera
     * @param parent Obiekt rodzica (opcjonalny)
     */
    ApiClient(const std::string &serverAddress, int port, QObject *parent = nullptr);

    /**
     * @brief Wykonuje żądanie HTTP do serwera
     * @param method Metoda HTTP (GET, POST, PUT, DELETE)
     * @param endpoint Punkt końcowy API
     * @param data Dane JSON do wysłania (opcjonalne)
     * @return Odpowiedź JSON z serwera
     */
    json executeRequest(const QString &method, const QString &endpoint, const json &data = json::object());

    /**
     * @brief Pobiera listę budynków
     * @return Wektor obiektów Building
     */
    std::vector<Building> getBuildings();

    /**
     * @brief Pobiera listę biurek
     * @param buildingId ID budynku (opcjonalnie)
     * @param floor Piętro (opcjonalnie)
     * @return Wektor obiektów Desk
     */
    std::vector<Desk> getDesks(int buildingId = -1, int floor = -1);

    /**
     * @brief Dodaje rezerwację
     * @param deskId ID biurka
     * @param userId ID użytkownika
     * @param dateFrom Data początkowa (format: yyyy-MM-dd)
     * @param dateTo Data końcowa (format: yyyy-MM-dd)
     * @return Para (sukces, komunikat błędu)
     */
    std::pair<bool, QString> addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    /**
     * @brief Anuluje rezerwację
     * @param bookingId ID rezerwacji
     * @return Czy operacja się powiodła
     */
    bool cancelBooking(int bookingId);

    /**
     * @brief Rejestruje nowego użytkownika
     * @param username Nazwa użytkownika
     * @param password Hasło
     * @param email Adres email
     * @return Opcjonalny obiekt User (brak w przypadku błędu)
     */
    std::optional<User> registerUser(const std::string &username, const std::string &password,
                                     const std::string &email);

    /**
     * @brief Loguje użytkownika
     * @param username Nazwa użytkownika
     * @param password Hasło
     * @return Opcjonalny obiekt User (brak w przypadku błędu)
     */
    std::optional<User> loginUser(const std::string &username, const std::string &password);

    /**
     * @brief Pobiera aktualnie zalogowanego użytkownika
     * @return Opcjonalny obiekt User
     */
    std::optional<User> getCurrentUser() const { return _currentUser; }

    /**
     * @brief Wylogowuje użytkownika
     */
    void logoutUser() { _currentUser = std::nullopt; }

    /**
     * @brief Sprawdza czy użytkownik jest zalogowany
     * @return Czy użytkownik jest zalogowany
     */
    bool isLoggedIn() const { return _currentUser.has_value(); }

signals:
    /**
     * @brief Sygnał emitowany po zakończeniu żądania
     */
    void requestCompleted();

    /**
     * @brief Sygnał emitowany w przypadku błędu sieciowego
     * @param error Komunikat błędu
     */
    void networkError(const QString &error);

private:
    QString _serverUrl;
    QNetworkAccessManager _networkManager;
    std::optional<User> _currentUser;
};

#endif
