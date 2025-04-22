#include "api_client.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QMessageBox>

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
    QUrl url(_serverUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    LOG_INFO("API Request: {} {}", method.toStdString(), url.toString().toStdString());

    QByteArray requestData;
    if (!data.empty()) {
        requestData = QByteArray::fromStdString(data.dump());
        LOG_INFO("Request data: {}", requestData.toStdString());
    }

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

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        LOG_ERROR("Network error {}: {}", statusCode, errorMsg.toStdString());
        // Don't emit the signal here to avoid multiple error messages
        // emit networkError(errorMsg);
        reply->deleteLater();
        return {{"status", "error"}, {"message", errorMsg.toStdString()}};
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    LOG_INFO("Response received: {}", responseData.toStdString());

    if (responseData.isEmpty()) {
        LOG_ERROR("Empty response");
        return {{"status", "error"}, {"message", "Empty response"}};
    }

    try {
        return json::parse(responseData.toStdString());
    } catch (const std::exception &e) {
        LOG_ERROR("JSON parse error: {}", e.what());
        return {{"status", "error"}, {"message", "Invalid response format"}};
    }
}

std::vector<Building> ApiClient::getBuildings() {
    json response = executeRequest("GET", "/api/buildings");
    std::vector<Building> buildings;

    if (response.contains("buildings") && response["buildings"].is_array()) {
        for (const auto &buildingJson: response["buildings"]) {
            int id = buildingJson.contains("id") ? buildingJson["id"].get<int>() : 0;
            std::string name = buildingJson.contains("name") ? buildingJson["name"].get<std::string>() : "Building";
            std::string address = buildingJson.contains("address") ? buildingJson["address"].get<std::string>() : "";
            int numFloors = buildingJson.contains("numFloors") ? buildingJson["numFloors"].get<int>() : 1;

            LOG_INFO("Building {} ({}) has {} floors", id, name, numFloors);
            Building building(id, name, address, numFloors);
            buildings.push_back(building);
        }
    }

    emit requestCompleted();
    return buildings;
}

std::vector<Desk> ApiClient::getDesks(int buildingId, int floor) {
    QString endpoint = "/api/desks";

    if (buildingId > 0) {
        endpoint += "?buildingId=" + QString::number(buildingId);

        if (floor > 0) {
            endpoint += "&floor=" + QString::number(floor);
        }
    }

    json response = executeRequest("GET", endpoint);
    std::vector<Desk> desks;

    if (response.contains("desks") && response["desks"].is_array()) {
        for (const auto &deskJson: response["desks"]) {
            int id = deskJson.contains("id") ? deskJson["id"].get<int>() : 0;
            std::string name = deskJson.contains("name") ? deskJson["name"].get<std::string>() : "Desk";
            int buildingId = deskJson.contains("buildingId") ? deskJson["buildingId"].get<int>() : 1;
            int floor = deskJson.contains("floor") ? deskJson["floor"].get<int>() : 1;

            Desk desk(id, name, buildingId, floor);

            if (deskJson.contains("bookings") && deskJson["bookings"].is_array()) {
                for (const auto &bookingJson: deskJson["bookings"]) {
                    desk.addBooking(Booking::fromJson(bookingJson));
                }
            }

            desks.push_back(desk);
        }
    }

    emit requestCompleted();
    return desks;
}

std::pair<bool, QString> ApiClient::addBooking(int deskId, int userId, const std::string &dateFrom,
                                               const std::string &dateTo) {
    if (!isLoggedIn()) {
        LOG_ERROR("Attempted to book desk when not logged in");
        return {false, "You must be logged in to book a desk"};
    }

    json data = {
        {"deskId", deskId},
        {"userId", userId},
        {"dateFrom", dateFrom},
        {"dateTo", dateTo}
    };

    LOG_INFO("Sending booking request: {}", data.dump());
    json response = executeRequest("POST", "/api/bookings", data);

    bool success = response.contains("status") && response["status"] == "success";

    if (!success) {
        QString errorMsg = "Could not book the desk.";

        if (response.contains("message")) {
            std::string serverMsg = response["message"].get<std::string>();
            LOG_ERROR("Booking error: {}", serverMsg);

            if (serverMsg.find("already booked") != std::string::npos) {
                errorMsg = "This desk is already booked for the selected date(s).";
            }
        }

        return {false, errorMsg};
    }

    return {true, ""};
}

bool ApiClient::cancelBooking(int bookingId) {
    if (!isLoggedIn()) return false;

    QString endpoint = "/api/bookings/" + QString::number(bookingId);
    json response = executeRequest("DELETE", endpoint);
    return response.contains("status") && response["status"] == "success";
}

std::optional<User> ApiClient::registerUser(const std::string &username, const std::string &password,
                                            const std::string &email) {
    json data = {
        {"username", username},
        {"password", password},
        {"email", email}
    };

    json response = executeRequest("POST", "/api/users/register", data);

    if (response.contains("status") && response["status"] == "success" &&
        response.contains("user") && !response["user"].is_null()) {
        auto &userJson = response["user"];
        int id = userJson.contains("id") ? userJson["id"].get<int>() : 0;
        std::string username = userJson.contains("username") ? userJson["username"].get<std::string>() : "";
        std::string email = userJson.contains("email") ? userJson["email"].get<std::string>() : "";

        User user(id, username, email);
        _currentUser = user;
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

        std::string username = userJson.contains("username") ? userJson["username"].get<std::string>() : "";
        std::string email = userJson.contains("email") ? userJson["email"].get<std::string>() : "";

        User user(id, username, email);
        _currentUser = user;
        return user;
    }

    return std::nullopt;
}
