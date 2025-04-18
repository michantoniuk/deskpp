#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "controller.h"
#include "../../service/user_service.h"

class UserController : public Controller {
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

#endif // USER_CONTROLLER_H
