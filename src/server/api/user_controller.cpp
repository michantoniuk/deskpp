#include "user_controller.h"
#include "../util/logger.h"

UserController::UserController(UserService &userService)
    : _userService(userService) {
}

crow::response UserController::getUsers(const crow::request &req) {
    try {
        json result = _userService.getAllUsers();
        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error getting users: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::getUser(int id) {
    try {
        json result = _userService.getUserById(id);
        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "User not found";
            return errorResponse(404, message);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error getting user: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::registerUser(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"username", "password", "email", "fullName"});
        if (!params) {
            return errorResponse(400, "Missing or invalid required fields");
        }

        std::string username = (*params)["username"].get<std::string>();
        std::string password = (*params)["password"].get<std::string>();
        std::string email = (*params)["email"].get<std::string>();
        std::string fullName = (*params)["fullName"].get<std::string>();

        LOG_INFO("Registering user: username={}, email={}", username, email);

        json result = _userService.registerUser(username, password, email, fullName);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error registering user";
            return errorResponse(400, message);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error registering user: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::loginUser(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"username", "password"});
        if (!params) {
            return errorResponse(400, "Missing or invalid required fields");
        }

        std::string username = (*params)["username"].get<std::string>();
        std::string password = (*params)["password"].get<std::string>();

        LOG_INFO("Login attempt: username={}", username);

        json result = _userService.loginUser(username, password);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Invalid login credentials";
            return errorResponse(401, message);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error during login: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::updateUser(int id, const crow::request &req) {
    try {
        json userData = parseJson(req.body);

        LOG_INFO("Updating user: id={}", id);

        json result = _userService.updateUser(id, userData);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error updating user";
            return errorResponse(400, message);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error updating user: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::deleteUser(int id) {
    try {
        LOG_INFO("Deleting user: id={}", id);

        json result = _userService.deleteUser(id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error deleting user";
            return errorResponse(404, message);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error deleting user: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

json UserController::parseJson(const std::string &body) {
    try {
        return json::parse(body);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error parsing JSON: {}", ex.what());
        throw std::runtime_error("Invalid JSON format");
    }
}

crow::response UserController::errorResponse(int statusCode, const std::string &message) {
    json response = {
        {"status", "error"},
        {"message", message}
    };

    return crow::response(statusCode, response.dump());
}

crow::response UserController::successResponse(const json &data) {
    return crow::response(200, data.dump());
}

std::optional<json> UserController::validateRequest(const crow::request &req,
                                                  const std::vector<std::string> &requiredFields) {
    try {
        json params = parseJson(req.body);

        // Check required fields
        for (const auto &field: requiredFields) {
            if (!params.contains(field) || params[field].is_null()) {
                return std::nullopt;
            }
        }

        return params;
    } catch (...) {
        return std::nullopt;
    }
}