#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QNetworkAccessManager>
#include <QObject>
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "common/model/model.h"

using json = nlohmann::json;

class ApiClient : public QObject {
    Q_OBJECT

public:
    ApiClient(QObject *parent = nullptr);

    ApiClient(const std::string &serverAddress, int port, QObject *parent = nullptr);

    // Data operations
    std::vector<Desk> getDesks(int buildingId = 1);

    json executeRequest(const QString &method, const QString &endpoint, const json &data = json::object());

    // Booking operations
    bool addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    bool cancelBooking(int bookingId);

    // User operations
    std::optional<User> registerUser(const std::string &username, const std::string &password,
                                     const std::string &email, const std::string &fullName);

    std::optional<User> loginUser(const std::string &username, const std::string &password);

    // User state
    std::optional<User> getCurrentUser() const { return _currentUser; }
    void logoutUser() { _currentUser = std::nullopt; }
    bool isLoggedIn() const { return _currentUser.has_value(); }

    // Admin operations
    bool isAdmin() const { return _isAdmin; }
    void setAdminMode(bool isAdmin) { _isAdmin = isAdmin; }

    // Building management
    bool addBuilding(const std::string &name, const std::string &address);

    bool updateBuilding(int id, const std::string &name, const std::string &address);

    bool deleteBuilding(int id);

    // Desk management
    bool addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX = 0, int locationY = 0);

    bool updateDesk(int id, const std::string &deskId, int buildingId, int floorNumber, int locationX = 0,
                    int locationY = 0);

    bool deleteDesk(int id);

private:
    QString _serverUrl;
    QNetworkAccessManager _networkManager;
    std::optional<User> _currentUser;
    bool _isAdmin = false;
};

#endif // API_CLIENT_H
