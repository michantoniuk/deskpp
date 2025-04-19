#include "api_client.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include "common/logger.h"

ApiClient::ApiClient(QObject *parent)
    : QObject(parent), _serverUrl("http://localhost:8080") {
}

ApiClient::ApiClient(const std::string &serverAddress, int port, QObject *parent)
    : QObject(parent) {
    _serverUrl = QString("http://%1:%2").arg(QString::fromStdString(serverAddress)).arg(port);
    LOG_INFO("API URL: {}", _serverUrl.toStdString());
}

json ApiClient::executeRequest(const QString &method, const QString &endpoint, const json &data) {
    // Create request
    QUrl url(_serverUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    LOG_INFO("Request: {} {}", method.toStdString(), endpoint.toStdString());

    // Prepare data
    QByteArray requestData;
    if (!data.empty()) {
        requestData = QByteArray::fromStdString(data.dump());
    }

    // Execute request
    QNetworkReply *reply = nullptr;
    if (method == "GET") {
        reply = _networkManager.get(request);
    } else if (method == "POST") {
        reply = _networkManager.post(request, requestData);
    } else if (method == "PUT") {
        reply = _networkManager.put(request, requestData);
    } else if (method == "DELETE") {
        reply = _networkManager.deleteResource(request);
    }

    // Wait for response
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // Check for network errors
    if (reply->error() != QNetworkReply::NoError) {
        LOG_ERROR("Network error: {}", reply->errorString().toStdString());
        reply->deleteLater();
        return {{"status", "error"}, {"message", reply->errorString().toStdString()}};
    }

    // Parse response
    QByteArray responseData = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();

    LOG_INFO("Response: status={}", statusCode);

    // Handle empty response
    if (responseData.isEmpty()) {
        return {{"status", "error"}, {"message", "Empty response"}};
    }

    try {
        return json::parse(responseData.toStdString());
    } catch (const std::exception &e) {
        LOG_ERROR("JSON parse error: {}", e.what());
        return {{"status", "error"}, {"message", "Invalid response format"}};
    }
}

bool ApiClient::checkAuthentication(const std::string &action) {
    if (!isLoggedIn()) {
        LOG_WARNING("Authentication required for: {}", action);
        return false;
    }
    return true;
}

std::vector<Desk> ApiClient::getDesks(int buildingId) {
    QString endpoint = "/api/desks";
    if (buildingId > 0) {
        endpoint += "?buildingId=" + QString::number(buildingId);
    }

    json response = executeRequest("GET", endpoint);
    std::vector<Desk> desks;

    if (response.contains("desks") && response["desks"].is_array()) {
        for (const auto &deskJson: response["desks"]) {
            int id = deskJson.contains("id") ? deskJson["id"].get<int>() : 0;
            std::string deskId = deskJson.contains("deskId") ? deskJson["deskId"].get<std::string>() : "Desk";
            std::string buildingId = "1";

            if (deskJson.contains("buildingId")) {
                if (deskJson["buildingId"].is_string())
                    buildingId = deskJson["buildingId"].get<std::string>();
                else if (deskJson["buildingId"].is_number())
                    buildingId = std::to_string(deskJson["buildingId"].get<int>());
            }

            int floorNumber = deskJson.contains("floorNumber") ? deskJson["floorNumber"].get<int>() : 1;
            int locationX = deskJson.contains("locationX") ? deskJson["locationX"].get<int>() : 0;
            int locationY = deskJson.contains("locationY") ? deskJson["locationY"].get<int>() : 0;

            Desk desk(id, deskId, buildingId, floorNumber, locationX, locationY);

            // Add bookings if available
            if (deskJson.contains("bookings") && deskJson["bookings"].is_array()) {
                for (const auto &bookingJson: deskJson["bookings"]) {
                    desk.addBooking(Booking::fromJson(bookingJson));
                }
            }

            desks.push_back(desk);
        }
    }

    return desks;
}

bool ApiClient::addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo) {
    if (!checkAuthentication("book a desk")) return false;

    json data = {
        {"deskId", deskId},
        {"userId", userId},
        {"dateFrom", dateFrom},
        {"dateTo", dateTo}
    };

    json response = executeRequest("POST", "/api/bookings", data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::cancelBooking(int bookingId) {
    if (!checkAuthentication("cancel a booking")) return false;

    QString endpoint = "/api/bookings/" + QString::number(bookingId);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}

std::optional<User> ApiClient::registerUser(const std::string &username, const std::string &password,
                                            const std::string &email, const std::string &fullName) {
    json data = {
        {"username", username},
        {"password", password},
        {"email", email},
        {"fullName", fullName}
    };

    json response = executeRequest("POST", "/api/users/register", data);

    if (response.contains("status") && response["status"] == "success" &&
        response.contains("user") && !response["user"].is_null()) {
        auto &userJson = response["user"];
        int id = userJson.contains("id") ? userJson["id"].get<int>() : 0;
        std::string username = userJson.contains("username") ? userJson["username"].get<std::string>() : "";
        std::string email = userJson.contains("email") ? userJson["email"].get<std::string>() : "";
        std::string fullName = userJson.contains("fullName") ? userJson["fullName"].get<std::string>() : "";

        User user(id, username, email, fullName);
        _currentUser = user;
        LOG_INFO("User registered: {}", username);
        return user;
    }

    return std::nullopt;
}

std::optional<User> ApiClient::loginUser(const std::string &username, const std::string &password) {
    json data = {
        {"username", username},
        {"password", password}
    };

    json response = executeRequest("POST", "/api/users/login", data);

    if (response.contains("status") && response["status"] == "success" &&
        response.contains("user") && !response["user"].is_null()) {
        auto &userJson = response["user"];
        int id = 0;

        // Get user ID
        if (userJson.contains("id") && !userJson["id"].is_null()) {
            if (userJson["id"].is_number()) {
                id = userJson["id"].get<int>();
            } else if (userJson["id"].is_string()) {
                try {
                    id = std::stoi(userJson["id"].get<std::string>());
                } catch (...) {
                    LOG_ERROR("Failed to parse user ID from string");
                }
            }
        }

        LOG_INFO("User login successful. User ID: {}", id);

        std::string username = userJson.contains("username") ? userJson["username"].get<std::string>() : "";
        std::string email = userJson.contains("email") ? userJson["email"].get<std::string>() : "";
        std::string fullName = userJson.contains("fullName") ? userJson["fullName"].get<std::string>() : "";

        User user(id, username, email, fullName);
        _currentUser = user;
        return user;
    }

    return std::nullopt;
}

bool ApiClient::addBuilding(const std::string &name, const std::string &address) {
    if (!checkAuthentication("manage buildings")) return false;

    json data = {
        {"name", name},
        {"address", address}
    };

    json response = executeRequest("POST", "/api/admin/buildings", data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::updateBuilding(int id, const std::string &name, const std::string &address) {
    if (!checkAuthentication("manage buildings")) return false;

    json data = {
        {"name", name},
        {"address", address}
    };

    QString endpoint = "/api/admin/buildings/" + QString::number(id);
    json response = executeRequest("PUT", endpoint, data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::deleteBuilding(int id) {
    if (!checkAuthentication("manage buildings")) return false;

    QString endpoint = "/api/admin/buildings/" + QString::number(id);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX, int locationY) {
    if (!checkAuthentication("manage desks")) return false;

    json data = {
        {"deskId", deskId},
        {"buildingId", buildingId},
        {"floorNumber", floorNumber},
        {"locationX", locationX},
        {"locationY", locationY}
    };

    json response = executeRequest("POST", "/api/admin/desks", data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::updateDesk(int id, const std::string &deskId, int buildingId, int floorNumber, int locationX,
                           int locationY) {
    if (!checkAuthentication("manage desks")) return false;

    json data = {
        {"deskId", deskId},
        {"buildingId", buildingId},
        {"floorNumber", floorNumber},
        {"locationX", locationX},
        {"locationY", locationY}
    };

    QString endpoint = "/api/admin/desks/" + QString::number(id);
    json response = executeRequest("PUT", endpoint, data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::deleteDesk(int id) {
    if (!checkAuthentication("manage desks")) return false;

    QString endpoint = "/api/admin/desks/" + QString::number(id);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}
