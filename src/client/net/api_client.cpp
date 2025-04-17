#include "api_client.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QEventLoop>
#include <QNetworkReply>
#include <QTimer>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent), _serverUrl("http://localhost:8080") {
}

ApiClient::ApiClient(const std::string &serverAddress, int port, QObject *parent)
    : QObject(parent) {
    _serverUrl = QString::fromStdString(
        std::string("http://") + serverAddress + ":" + std::to_string(port)
    );
}

json ApiClient::executeRequest(const QString &method, const QString &endpoint, const json &data) {
    // Create request
    QUrl url(_serverUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

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

    // Parse response
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    return json::parse(responseData.toStdString());
}

std::vector<Desk> ApiClient::getDesks(int buildingId) {
    QString endpoint = "/api/desks";
    if (buildingId > 0) {
        endpoint += "?buildingId=" + QString::number(buildingId);
    }

    std::vector<Desk> desks;
    json response = executeRequest("GET", endpoint);

    if (response.contains("desks") && response["desks"].is_array()) {
        for (const auto &deskJson: response["desks"]) {
            int id = deskJson.contains("id") ? deskJson["id"].get<int>() : 0;
            std::string deskId = deskJson.contains("deskId") ? deskJson["deskId"].get<std::string>() : "Desk";
            std::string buildingId = "1";
            if (deskJson.contains("buildingId")) {
                if (deskJson["buildingId"].is_string()) {
                    buildingId = deskJson["buildingId"].get<std::string>();
                } else if (deskJson["buildingId"].is_number()) {
                    buildingId = std::to_string(deskJson["buildingId"].get<int>());
                }
            }

            int floorNumber = deskJson.contains("floorNumber") ? deskJson["floorNumber"].get<int>() : 1;

            // Create desk
            Desk desk(id, deskId, buildingId, floorNumber);

            // Set coordinates
            desk.setLocationX(deskJson.contains("locationX") ? deskJson["locationX"].get<int>() : 0);
            desk.setLocationY(deskJson.contains("locationY") ? deskJson["locationY"].get<int>() : 0);

            desks.push_back(desk);
        }
    }

    return desks;
}

bool ApiClient::addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo) {
    const json data = {
        {"deskId", deskId},
        {"userId", userId},
        {"dateFrom", dateFrom},
        {"dateTo", dateTo}
    };

    json response = executeRequest("POST", "/api/bookings", data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::cancelBooking(int bookingId) {
    QString endpoint = "/api/bookings/" + QString::number(bookingId);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}

std::optional<User> ApiClient::registerUser(const std::string &username, const std::string &password,
                                            const std::string &email, const std::string &fullName) {
    const json data = {
        {"username", username},
        {"password", password},
        {"email", email},
        {"fullName", fullName}
    };

    json response = executeRequest("POST", "/api/users/register", data);

    if (response.contains("user") && !response["user"].is_null()) {
        auto &userJson = response["user"];
        int id = userJson.contains("id") ? userJson["id"].get<int>() : 1;
        std::string username = userJson.contains("username") ? userJson["username"].get<std::string>() : "";
        std::string email = userJson.contains("email") ? userJson["email"].get<std::string>() : "";
        std::string fullName = userJson.contains("fullName") ? userJson["fullName"].get<std::string>() : "";

        User user(id, username, email, fullName);
        _currentUser = user;
        return user;
    }

    return std::nullopt;
}

std::optional<User> ApiClient::loginUser(const std::string &username, const std::string &password) {
    const json data = {
        {"username", username},
        {"password", password}
    };

    json response = executeRequest("POST", "/api/users/login", data);

    if (response.contains("user") && !response["user"].is_null()) {
        auto &userJson = response["user"];
        int id = userJson.contains("id") ? userJson["id"].get<int>() : 1;
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
    const json data = {
        {"name", name},
        {"address", address}
    };

    json response = executeRequest("POST", "/api/admin/buildings", data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::updateBuilding(int id, const std::string &name, const std::string &address) {
    const json data = {
        {"name", name},
        {"address", address}
    };

    QString endpoint = "/api/admin/buildings/" + QString::number(id);
    json response = executeRequest("PUT", endpoint, data);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::deleteBuilding(int id) {
    QString endpoint = "/api/admin/buildings/" + QString::number(id);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}

bool ApiClient::addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX, int locationY) {
    const json data = {
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
    const json data = {
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
    QString endpoint = "/api/admin/desks/" + QString::number(id);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}
