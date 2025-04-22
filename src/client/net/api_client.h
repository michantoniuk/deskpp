#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QNetworkAccessManager>
#include <QObject>
#include <optional>
#include "common/model/model.h"

class ApiClient : public QObject {
    Q_OBJECT

public:
    ApiClient(QObject *parent = nullptr);

    ApiClient(const std::string &serverAddress, int port, QObject *parent = nullptr);

    // Network operations
    json executeRequest(const QString &method, const QString &endpoint, const json &data = json::object());

    // Data operations
    std::vector<Building> getBuildings();

    std::vector<Desk> getDesks(int buildingId = -1, int floor = -1);

    // Booking operations
    std::pair<bool, QString> addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    bool cancelBooking(int bookingId);

    // User operations
    std::optional<User> registerUser(const std::string &username, const std::string &password,
                                     const std::string &email);

    std::optional<User> loginUser(const std::string &username, const std::string &password);

    // User state
    std::optional<User> getCurrentUser() const { return _currentUser; }
    void logoutUser() { _currentUser = std::nullopt; }
    bool isLoggedIn() const { return _currentUser.has_value(); }

signals:
    void requestCompleted();

    void networkError(const QString &error);

private:
    QString _serverUrl;
    QNetworkAccessManager _networkManager;
    std::optional<User> _currentUser;
};

#endif
