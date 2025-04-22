#include "user_controller.h"

UserController::UserController(UserService &userService)
    : _userService(userService) {
}

crow::response UserController::registerUser(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"username", "password", "email"});
        if (!params) {
            return errorResponse(400, "Brakujące wymagane pola");
        }

        std::string username = (*params)["username"].get<std::string>();
        std::string password = (*params)["password"].get<std::string>();
        std::string email = (*params)["email"].get<std::string>();

        json result = _userService.registerUser(username, password, email);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Błąd serwera");
    }
}

crow::response UserController::loginUser(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"username", "password"});
        if (!params) {
            return errorResponse(400, "Brakujące wymagane pola");
        }

        std::string username = (*params)["username"].get<std::string>();
        std::string password = (*params)["password"].get<std::string>();

        json result = _userService.loginUser(username, password);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(401, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Błąd serwera");
    }
}
