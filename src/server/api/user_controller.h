#ifndef SERVER_USER_CONTROLLER_H
#define SERVER_USER_CONTROLLER_H

#include "base_controller.h"
#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include "../service/user_service.h"

using json = nlohmann::json;

/**
 * Controller for user-related API endpoints
 */
class UserController : public BaseController {
public:
    explicit UserController(UserService &userService);

    crow::response getUsers(const crow::request &req);

    crow::response getUser(int id);

    crow::response registerUser(const crow::request &req);

    crow::response loginUser(const crow::request &req);

    crow::response updateUser(int id, const crow::request &req);

    crow::response deleteUser(int id);

private:
    UserService &_userService;
};

#endif // SERVER_USER_CONTROLLER_H
