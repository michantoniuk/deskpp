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
#include "../util/logger.h"

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

                // Process bookings if present
                if (deskJson.contains("bookings") && deskJson["bookings"].is_array()) {
                    // Process multiple bookings
                    for (const auto &bookingJson: deskJson["bookings"]) {
                        try {
                            Booking booking = Booking::fromJson(bookingJson);

                            // Only add valid bookings
                            if (booking.getDateFrom().isValid() && booking.getDateTo().isValid()) {
                                // Set the desk ID if not already set
                                if (booking.getDeskId() == 0) {
                                    booking.setDeskId(id);
                                }
                                desk.addBooking(booking);
                            }
                        } catch (const std::exception &e) {
                            LOG_ERROR("Error processing booking JSON: {}", e.what());
                        }
                    }
                } else if (deskJson.contains("booked") && deskJson["booked"].get<bool>()) {
                    // Handle legacy single booking information
                    int bookingId = deskJson.contains("bookingId") ? deskJson["bookingId"].get<int>() : 0;

                    QDate dateFrom, dateTo;

                    if (deskJson.contains("bookingDateFrom") && !deskJson["bookingDateFrom"].is_null()) {
                        std::string dateFromStr = deskJson["bookingDateFrom"].get<std::string>();
                        dateFrom = QDate::fromString(QString::fromStdString(dateFromStr), "yyyy-MM-dd");
                    } else if (deskJson.contains("bookingDate") && !deskJson["bookingDate"].is_null()) {
                        // For compatibility with old format
                        std::string dateStr = deskJson["bookingDate"].get<std::string>();
                        dateFrom = QDate::fromString(QString::fromStdString(dateStr), "yyyy-MM-dd");
                    } else {
                        dateFrom = QDate::currentDate();
                    }

                    if (deskJson.contains("bookingDateTo") && !deskJson["bookingDateTo"].is_null()) {
                        std::string dateToStr = deskJson["bookingDateTo"].get<std::string>();
                        dateTo = QDate::fromString(QString::fromStdString(dateToStr), "yyyy-MM-dd");
                    } else {
                        dateTo = dateFrom; // If no end date, use start date
                    }

                    // Create a Booking object and add it to the desk
                    if (dateFrom.isValid() && dateTo.isValid()) {
                        Booking booking(bookingId, id, 0, dateFrom, dateTo);
                        desk.addBooking(booking);
                    }
                }

                desks.push_back(desk);
            }

            LOG_INFO("Retrieved {} desks", desks.size());
        } else {
            std::string errorMsg = response.contains("message")
                                       ? response["message"].get<std::string>()
                                       : "Unknown error";
            LOG_ERROR("Error retrieving desks: {}", errorMsg);
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

    try {
        json response = executeRequest("POST", "/api/bookings", data);

        bool success = response.contains("status") && response["status"] == "success";

        if (success) {
            LOG_INFO("Booking added successfully");
        } else {
            std::string errorMsg = response.contains("message")
                                       ? response["message"].get<std::string>()
                                       : "Unknown error";
            LOG_ERROR("Error adding booking: {}", errorMsg);
        }

        return success;
    } catch (const std::exception &ex) {
        LOG_ERROR("Error adding booking: {}", ex.what());
        return false;
    }
}

bool ApiClient::cancelBooking(int bookingId) {
    LOG_INFO("Canceling booking: id={}", bookingId);

    try {
        QString endpoint = "/api/bookings/" + QString::number(bookingId);
        json response = executeRequest("DELETE", endpoint);

        bool success = response.contains("status") && response["status"] == "success";

        if (success) {
            LOG_INFO("Booking canceled successfully");
        } else {
            std::string errorMsg = response.contains("message")
                                       ? response["message"].get<std::string>()
                                       : "Unknown error";
            LOG_ERROR("Error canceling booking: {}", errorMsg);
        }

        return success;
    } catch (const std::exception &ex) {
        LOG_ERROR("Error canceling booking: {}", ex.what());
        return false;
    }
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

    try {
        json response = executeRequest("POST", "/api/users/register", data);

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("user") && !response["user"].is_null()) {
            // Create User from response
            int id = response["user"].contains("id") ? response["user"]["id"].get<int>() : 0;
            std::string username = response["user"].contains("username")
                                       ? response["user"]["username"].get<std::string>()
                                       : "";
            std::string email = response["user"].contains("email") ? response["user"]["email"].get<std::string>() : "";
            std::string fullName = response["user"].contains("fullName")
                                       ? response["user"]["fullName"].get<std::string>()
                                       : "";

            User user(id, username, email, fullName);

            LOG_INFO("User registered successfully: id={}", user.getId());

            // Set as current user
            _currentUser = user;

            return user;
        } else {
            std::string errorMsg = response.contains("message")
                                       ? response["message"].get<std::string>()
                                       : "Unknown error";
            LOG_ERROR("Error registering user: {}", errorMsg);
            return std::nullopt;
        }
    } catch (const std::exception &ex) {
        LOG_ERROR("Error registering user: {}", ex.what());
        return std::nullopt;
    }
}

std::optional<User> ApiClient::loginUser(const std::string &username, const std::string &password) {
    LOG_INFO("Logging in user: username={}", username);

    const json data = {
        {"username", username},
        {"password", password}
    };

    try {
        json response = executeRequest("POST", "/api/users/login", data);

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("user") && !response["user"].is_null()) {
            // Create User from response
            int id = response["user"].contains("id") ? response["user"]["id"].get<int>() : 0;
            std::string username = response["user"].contains("username")
                                       ? response["user"]["username"].get<std::string>()
                                       : "";
            std::string email = response["user"].contains("email") ? response["user"]["email"].get<std::string>() : "";
            std::string fullName = response["user"].contains("fullName")
                                       ? response["user"]["fullName"].get<std::string>()
                                       : "";

            User user(id, username, email, fullName);

            LOG_INFO("User logged in successfully: id={}", user.getId());

            // Set as current user
            _currentUser = user;

            return user;
        } else {
            std::string errorMsg = response.contains("message")
                                       ? response["message"].get<std::string>()
                                       : "Unknown error";
            LOG_ERROR("Error logging in user: {}", errorMsg);
            return std::nullopt;
        }
    } catch (const std::exception &ex) {
        LOG_ERROR("Error logging in user: {}", ex.what());
        return std::nullopt;
    }
}
