#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "controller.h"
#include "../../service/user_service.h"

class UserController : public Controller {
public:
    explicit UserController(UserService &userService);

    crow::response registerUser(const crow::request &req);

    crow::response loginUser(const crow::request &req);

private:
    UserService &_userService;
};

#endif
