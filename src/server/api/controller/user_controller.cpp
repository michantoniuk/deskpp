#include "user_controller.h"

UserController::UserController(UserService &userService)
    : _userService(userService) {
}

crow::response UserController::getUsers(const crow::request &req) {
    try {
        json result = _userService.getAllUsers();
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::getUser(int id) {
    try {
        json result = _userService.getUserById(id);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(404, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::registerUser(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"username", "password", "email", "fullName"});
        if (!params) {
            return errorResponse(400, "Missing required fields");
        }

        std::string username = (*params)["username"].get<std::string>();
        std::string password = (*params)["password"].get<std::string>();
        std::string email = (*params)["email"].get<std::string>();
        std::string fullName = (*params)["fullName"].get<std::string>();

        json result = _userService.registerUser(username, password, email, fullName);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::loginUser(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"username", "password"});
        if (!params) {
            return errorResponse(400, "Missing required fields");
        }

        std::string username = (*params)["username"].get<std::string>();
        std::string password = (*params)["password"].get<std::string>();

        json result = _userService.loginUser(username, password);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(401, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::updateUser(int id, const crow::request &req) {
    try {
        json userData = parseJson(req.body);
        json result = _userService.updateUser(id, userData);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(404, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response UserController::deleteUser(int id) {
    try {
        json result = _userService.deleteUser(id);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(404, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}
