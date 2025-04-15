#include "user_controller.h"
#include "common/logger.h"

UserController::UserController(UserService &userService)
    : _userService(userService) {
}

crow::response UserController::getUsers(const crow::request &req) {
    return tryCatchResponse([&]() {
        json result = _userService.getAllUsers();
        return successResponse(result);
    });
}

crow::response UserController::getUser(int id) {
    return tryCatchResponse([&]() {
        json result = _userService.getUserById(id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "User not found";
            return errorResponse(404, message);
        }

        return successResponse(result);
    });
}

crow::response UserController::registerUser(const crow::request &req) {
    return tryCatchResponse([&]() {
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
    });
}

crow::response UserController::loginUser(const crow::request &req) {
    return tryCatchResponse([&]() {
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
    });
}

crow::response UserController::updateUser(int id, const crow::request &req) {
    return tryCatchResponse([&]() {
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
    });
}

crow::response UserController::deleteUser(int id) {
    return tryCatchResponse([&]() {
        json result = _userService.deleteUser(id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "User not found";
            return errorResponse(404, message);
        }

        return successResponse(result);
    });
}
