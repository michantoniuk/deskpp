#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../service/user_service.h"

using json = nlohmann::json;

class UserController {
public:
    UserController(UserService &userService);

    crow::response getUsers(const crow::request &req);
    crow::response getUser(int id);
    crow::response registerUser(const crow::request &req);
    crow::response loginUser(const crow::request &req);
    crow::response updateUser(int id, const crow::request &req);
    crow::response deleteUser(int id);

private:
    UserService &_userService;

    json parseJson(const std::string &body);
    crow::response errorResponse(int statusCode, const std::string &message);
    crow::response successResponse(const json &data);
    std::optional<json> validateRequest(const crow::request &req,
                                        const std::vector<std::string> &requiredFields);
};

#endif // USER_CONTROLLER_H