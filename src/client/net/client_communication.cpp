#include "client_communication.h"
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "../util/logger.h"

using tcp = boost::asio::ip::tcp;

ClientCommunication::ClientCommunication()
    : _serverAddress("localhost"), _port(8080), _connected(false) {
    LOG_INFO("Creating default ClientCommunication instance (localhost:8080)");
}

ClientCommunication::ClientCommunication(const std::string &serverAddress, int port)
    : _serverAddress(serverAddress), _port(port), _connected(false) {
    LOG_INFO("Establishing connection with REST server address: {}, port: {}", serverAddress, port);
    testConnection();
}

ClientCommunication::~ClientCommunication() = default;

bool ClientCommunication::testConnection() {
    try {
        tcp::resolver resolver(_io_context);
        beast::tcp_stream stream(_io_context);

        auto const result = resolver.resolve(_serverAddress, std::to_string(_port));
        stream.expires_after(std::chrono::seconds(5)); // 5 second timeout
        stream.connect(result);

        beast::error_code errorCode;
        stream.socket().shutdown(tcp::socket::shutdown_both, errorCode);
        if (errorCode && errorCode != beast::errc::not_connected) {
            throw beast::system_error{errorCode};
        }

        _connected = true;
        LOG_INFO("Connection to server established successfully");
        return true;
    } catch (const std::exception &exception) {
        _connected = false;
        std::string errorMsg = exception.what();
        LOG_ERROR("Server connection error: {}", errorMsg);
        return false;
    }
}

json ClientCommunication::executeRequest(http::verb method, const std::string &endpoint, const json &data) {
    if (!_connected && !testConnection()) {
        return {{"status", "error"}, {"message", "No connection to server"}};
    }

    try {
        _io_context.restart();
        tcp::resolver resolver(_io_context);
        beast::tcp_stream stream(_io_context);

        auto const result = resolver.resolve(_serverAddress, std::to_string(_port));
        stream.expires_after(std::chrono::seconds(5)); // 5 second timeout
        stream.connect(result);

        http::request<http::string_body> request{method, endpoint, 11};
        request.set(http::field::host, _serverAddress);
        request.set(http::field::user_agent, "DeskPP-Client");

        if (method == http::verb::post) {
            request.set(http::field::content_type, "application/json");
            std::string body = data.dump();
            request.body() = body;
            request.set(http::field::content_length, std::to_string(body.size()));
        }

        http::write(stream, request);
        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(stream, buffer, response);

        beast::error_code errorCode;
        stream.socket().shutdown(tcp::socket::shutdown_both, errorCode);
        if (errorCode && errorCode != beast::errc::not_connected) {
            throw beast::system_error{errorCode};
        }

        if (response.result() == http::status::ok) {
            try {
                return json::parse(response.body());
            } catch (const std::exception &e) {
                std::string errorMsg = e.what();
                LOG_ERROR("JSON parsing error: {}", errorMsg);
                return {{"status", "error"}, {"message", "Invalid response format"}};
            }
        } else {
            std::string reasonStr = response.reason().to_string();
            LOG_ERROR("HTTP error: {} {}", response.result_int(), reasonStr);
            return {{"status", "error"}, {"message", "HTTP error: " + std::to_string(response.result_int())}};
        }
    } catch (const std::exception &e) {
        _connected = false;
        std::string errorMsg = e.what();
        LOG_ERROR("Communication error: {}", errorMsg);
        return {{"status", "error"}, {"message", std::string("Communication error: ") + errorMsg}};
    }
}

std::vector<json> ClientCommunication::getDesks(int buildingId) {
    LOG_INFO("Getting desks from server (building ID: {})", buildingId);

    std::string target = "/api/desks";
    if (buildingId > 0) {
        target += "?buildingId=" + std::to_string(buildingId);
    }

    json response = executeRequest(http::verb::get, target);

    std::vector<json> result;

    if (response.contains("status") && response["status"] == "success" &&
        response.contains("desks") && response["desks"].is_array()) {
        for (const auto &desk: response["desks"]) {
            result.push_back(desk);
        }
        LOG_INFO("Retrieved {} desks", result.size());
    } else {
        std::string errorMsg = response.contains("message")
                                   ? response["message"].get<std::string>()
                                   : "Unknown error";
        LOG_ERROR("Error retrieving desks: {}", errorMsg);
    }

    return result;
}

bool ClientCommunication::addBooking(int deskId, int userId, const std::string &dateFrom,
                                     const std::string &dateTo) {
    LOG_INFO("Adding booking: desk ID {}, user ID {}, from {} to {}",
             deskId, userId, dateFrom, dateTo);

    const json data = {
        {"deskId", deskId},
        {"userId", userId},
        {"dateFrom", dateFrom},
        {"dateTo", dateTo}
    };

    json response = executeRequest(http::verb::post, "/api/bookings", data);

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
}

bool ClientCommunication::cancelBooking(int bookingId) {
    LOG_INFO("Canceling booking: id={}", bookingId);

    std::string target = "/api/bookings/" + std::to_string(bookingId);

    json response = executeRequest(http::verb::delete_, target);

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
}

bool ClientCommunication::isConnected() const {
    return _connected;
}

// User-related methods
std::optional<User> ClientCommunication::registerUser(const std::string &username, const std::string &password,
                                                      const std::string &email, const std::string &fullName) {
    LOG_INFO("Registering user: username={}, email={}", username, email);

    const json data = {
        {"username", username},
        {"password", password},
        {"email", email},
        {"fullName", fullName}
    };

    json response = executeRequest(http::verb::post, "/api/users/register", data);

    if (response.contains("status") && response["status"] == "success" &&
        response.contains("user") && !response["user"].is_null()) {
        std::string userJson = response["user"].dump();
        User user = User::fromJson(userJson);

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
}

std::optional<User> ClientCommunication::loginUser(const std::string &username, const std::string &password) {
    LOG_INFO("Logging in user: username={}", username);

    const json data = {
        {"username", username},
        {"password", password}
    };

    json response = executeRequest(http::verb::post, "/api/users/login", data);

    if (response.contains("status") && response["status"] == "success" &&
        response.contains("user") && !response["user"].is_null()) {
        std::string userJson = response["user"].dump();
        User user = User::fromJson(userJson);

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
}

std::optional<User> ClientCommunication::getCurrentUser() const {
    return _currentUser;
}

void ClientCommunication::setCurrentUser(const User &user) {
    _currentUser = user;
}

void ClientCommunication::logoutUser() {
    LOG_INFO("User logged out");
    _currentUser = std::nullopt;
}

bool ClientCommunication::isLoggedIn() const {
    return _currentUser.has_value();
}
