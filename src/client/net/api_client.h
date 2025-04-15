#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "common/model/model.h"
#include "common/logger.h"

using json = nlohmann::json;

/**
 * API client for communication with the server.
 * Uses Qt's networking facilities.
 */
class ApiClient : public QObject {
    Q_OBJECT

public:
    ApiClient(QObject *parent = nullptr);

    ApiClient(const std::string &serverAddress, int port, QObject *parent = nullptr);

    ~ApiClient();

    // Connection management
    bool testConnection();

    bool isConnected() const { return _connected; }

    // Desk operations
    std::vector<Desk> getDesks(int buildingId = -1);

    // Booking operations
    bool addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    bool cancelBooking(int bookingId);

    // User operations
    std::optional<User> registerUser(const std::string &username, const std::string &password,
                                     const std::string &email, const std::string &fullName);

    std::optional<User> loginUser(const std::string &username, const std::string &password);

    // User state management
    std::optional<User> getCurrentUser() const { return _currentUser; }
    void setCurrentUser(const User &user) { _currentUser = user; }
    void logoutUser() { _currentUser = std::nullopt; }
    bool isLoggedIn() const { return _currentUser.has_value(); }

private:
    // HTTP request execution with wait for completion
    json executeRequest(const QString &method, const QString &endpoint, const json &data = json::object());

    // Simple request handlers
    std::optional<User> executeUserRequest(const QString &method, const QString &endpoint,
                                           const json &data = json::object());

    bool executeActionRequest(const QString &method, const QString &endpoint, const json &data = json::object());

    // Server information
    QString _serverUrl;
    bool _connected = false;

    // Networking
    QNetworkAccessManager _networkManager;

    // User state
    std::optional<User> _currentUser;
};

#endif // API_CLIENT_H
