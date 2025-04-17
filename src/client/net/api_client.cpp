#include "api_client.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QNetworkReply>
#include <QTimer>
#include <sstream>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent), _serverUrl("http://localhost:8080") {
    LOG_INFO("Creating default ApiClient (localhost:8080)");
    testConnection();
}

ApiClient::ApiClient(const std::string &serverAddress, int port, QObject *parent)
    : QObject(parent) {
    // Form the server URL
    std::ostringstream urlStream;
    urlStream << "http://" << serverAddress << ":" << port;
    _serverUrl = QString::fromStdString(urlStream.str());

    LOG_INFO("Creating ApiClient with server URL: {}", _serverUrl.toStdString());
    testConnection();
}

ApiClient::~ApiClient() {
    // QNetworkAccessManager cleans up after itself
}

bool ApiClient::testConnection() {
    try {
        // Make a simple GET request to the server
        json response = executeRequest("GET", "/api/buildings");

        // Check if the response is valid
        _connected = (response.contains("status") && response["status"] == "success");

        if (_connected) {
            LOG_INFO("Connection to server established successfully");
        } else {
            LOG_ERROR("Connection to server failed: Invalid response");
        }

        return _connected;
    } catch (const std::exception &ex) {
        _connected = false;
        LOG_ERROR("Connection to server failed: {}", ex.what());
        return false;
    }
}

json ApiClient::executeRequest(const QString &method, const QString &endpoint, const json &data) {
    // Create the request URL
    QUrl url(_serverUrl + endpoint);
    QNetworkRequest request(url);

    // Set up the request
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Prepare data if needed
    QByteArray requestData;
    if (!data.empty()) {
        requestData = QByteArray::fromStdString(data.dump());
    }

    // Execute the request based on method
    QNetworkReply *reply = nullptr;

    if (method == "GET") {
        reply = _networkManager.get(request);
    } else if (method == "POST") {
        reply = _networkManager.post(request, requestData);
    } else if (method == "PUT") {
        reply = _networkManager.put(request, requestData);
    } else if (method == "DELETE") {
        reply = _networkManager.deleteResource(request);
    } else {
        throw std::runtime_error("Unsupported HTTP method: " + method.toStdString());
    }

    // Wait for the request to complete
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    // Add timeout
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(5000); // 5 second timeout

    loop.exec();

    // Check for timeout
    if (!reply->isFinished()) {
        reply->abort();
        reply->deleteLater();
        throw std::runtime_error("Request timeout");
    }

    // Check for network errors
    if (reply->error() != QNetworkReply::NoError) {
        QString errorString = reply->errorString();
        reply->deleteLater();
        throw std::runtime_error("Network error: " + errorString.toStdString());
    }

    // Read the response
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    // Parse the JSON response
    try {
        return json::parse(responseData.toStdString());
    } catch (const std::exception &ex) {
        throw std::runtime_error("Invalid JSON response: " + std::string(ex.what()));
    }
}

std::optional<User> ApiClient::executeUserRequest(const QString &method, const QString &endpoint, const json &data) {
    try {
        json response = executeRequest(method, endpoint, data);

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("user") && !response["user"].is_null()) {
            auto &jsonObj = response["user"];
            int id = jsonObj.contains("id") ? jsonObj["id"].get<int>() : 0;
            std::string username = jsonObj.contains("username") ? jsonObj["username"].get<std::string>() : "";
            std::string email = jsonObj.contains("email") ? jsonObj["email"].get<std::string>() : "";
            std::string fullName = jsonObj.contains("fullName") ? jsonObj["fullName"].get<std::string>() : "";

            return User(id, username, email, fullName);
        }
    } catch (const std::exception &ex) {
        LOG_ERROR("Error in user request: {}", ex.what());
    }

    return std::nullopt;
}

bool ApiClient::executeActionRequest(const QString &method, const QString &endpoint, const json &data) {
    try {
        json response = executeRequest(method, endpoint, data);
        return response.contains("status") && response["status"] == "success";
    } catch (const std::exception &ex) {
        LOG_ERROR("Error in action request: {}", ex.what());
        return false;
    }
}

std::vector<Desk> ApiClient::getDesks(int buildingId) {
    LOG_INFO("Getting desks from server (building ID: {})", buildingId);

    QString endpoint = "/api/desks";
    if (buildingId > 0) {
        endpoint += "?buildingId=" + QString::number(buildingId);
    }

    std::vector<Desk> desks;
    try {
        json response = executeRequest("GET", endpoint);

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("desks") && response["desks"].is_array()) {
            for (const auto &deskJson: response["desks"]) {
                // Extract desk info
                int id = deskJson.contains("id") ? deskJson["id"].get<int>() : 0;
                std::string deskId = deskJson.contains("deskId") ? deskJson["deskId"].get<std::string>() : "Unknown";

                // Extract building ID
                std::string buildingId;
                if (deskJson.contains("buildingId")) {
                    if (deskJson["buildingId"].is_string()) {
                        buildingId = deskJson["buildingId"].get<std::string>();
                    } else if (deskJson["buildingId"].is_number()) {
                        buildingId = std::to_string(deskJson["buildingId"].get<int>());
                    } else {
                        buildingId = "Unknown";
                    }
                } else {
                    buildingId = "Unknown";
                }

                int floorNumber = deskJson.contains("floorNumber") ? deskJson["floorNumber"].get<int>() : 0;

                // Create desk with extracted info
                Desk desk(id, deskId, buildingId, floorNumber);

                // Add these two lines to extract coordinates
                desk.setLocationX(deskJson.contains("locationX") ? deskJson["locationX"].get<int>() : 0);
                desk.setLocationY(deskJson.contains("locationY") ? deskJson["locationY"].get<int>() : 0);

                // Rest of the function remains the same...
                // Process bookings, etc.

                desks.push_back(desk);
            }
        }
    } catch (const std::exception &ex) {
        LOG_ERROR("Error retrieving desks: {}", ex.what());
    }

    return desks;
}

bool ApiClient::addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo) {
    LOG_INFO("Adding booking: desk ID {}, user ID {}, from {} to {}", deskId, userId, dateFrom, dateTo);

    const json data = {
        {"deskId", deskId},
        {"userId", userId},
        {"dateFrom", dateFrom},
        {"dateTo", dateTo}
    };

    return executeActionRequest("POST", "/api/bookings", data);
}

bool ApiClient::cancelBooking(int bookingId) {
    LOG_INFO("Canceling booking: id={}", bookingId);
    QString endpoint = "/api/bookings/" + QString::number(bookingId);
    return executeActionRequest("DELETE", endpoint);
}

std::optional<User> ApiClient::registerUser(const std::string &username, const std::string &password,
                                            const std::string &email, const std::string &fullName) {
    LOG_INFO("Registering user: username={}, email={}", username, email);

    const json data = {
        {"username", username},
        {"password", password},
        {"email", email},
        {"fullName", fullName}
    };

    auto user = executeUserRequest("POST", "/api/users/register", data);

    if (user) {
        // Set as current user
        _currentUser = *user;
        LOG_INFO("User registered successfully: id={}", user->getId());
    }

    return user;
}

std::optional<User> ApiClient::loginUser(const std::string &username, const std::string &password) {
    LOG_INFO("Logging in user: username={}", username);

    const json data = {
        {"username", username},
        {"password", password}
    };

    auto user = executeUserRequest("POST", "/api/users/login", data);

    if (user) {
        // Set as current user
        _currentUser = *user;
        LOG_INFO("User logged in successfully: id={}", user->getId());
    }

    return user;
}

bool ApiClient::isAdmin() const {
    return _isAdmin;
}

void ApiClient::setAdminMode(bool isAdmin) {
    _isAdmin = isAdmin;
}

bool ApiClient::addBuilding(const std::string &name, const std::string &address) {
    LOG_INFO("Adding building: name={}, address={}", name, address);

    const json data = {
        {"name", name},
        {"address", address}
    };

    return executeActionRequest("POST", "/api/admin/buildings", data);
}

bool ApiClient::updateBuilding(int id, const std::string &name, const std::string &address) {
    LOG_INFO("Updating building: id={}, name={}, address={}", id, name, address);

    const json data = {
        {"name", name},
        {"address", address}
    };

    QString endpoint = "/api/admin/buildings/" + QString::number(id);
    return executeActionRequest("PUT", endpoint, data);
}

bool ApiClient::deleteBuilding(int id) {
    LOG_INFO("Deleting building: id={}", id);

    QString endpoint = "/api/admin/buildings/" + QString::number(id);
    return executeActionRequest("DELETE", endpoint);
}

bool ApiClient::addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX, int locationY) {
    LOG_INFO("Adding desk: deskId={}, buildingId={}, floor={}, position=({},{})",
             deskId, buildingId, floorNumber, locationX, locationY);

    const json data = {
        {"deskId", deskId},
        {"buildingId", buildingId},
        {"floorNumber", floorNumber},
        {"locationX", locationX},
        {"locationY", locationY}
    };

    return executeActionRequest("POST", "/api/admin/desks", data);
}

bool ApiClient::updateDesk(int id, const std::string &deskId, int buildingId, int floorNumber, int locationX,
                           int locationY) {
    LOG_INFO("Updating desk: id={}, deskId={}, buildingId={}, floor={}, position=({},{})",
             id, deskId, buildingId, floorNumber, locationX, locationY);

    const json data = {
        {"deskId", deskId},
        {"buildingId", buildingId},
        {"floorNumber", floorNumber},
        {"locationX", locationX},
        {"locationY", locationY}
    };

    QString endpoint = "/api/admin/desks/" + QString::number(id);
    return executeActionRequest("PUT", endpoint, data);
}

bool ApiClient::deleteDesk(int id) {
    LOG_INFO("Deleting desk: id={}", id);

    QString endpoint = "/api/admin/desks/" + QString::number(id);
    return executeActionRequest("DELETE", endpoint);
}
