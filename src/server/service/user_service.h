#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "service.h"
#include "../repository/user_repository.h"

class UserService : public Service<User> {
public:
    explicit UserService(UserRepository &userRepository);

    json registerUser(const std::string &username, const std::string &password, const std::string &email);

    json loginUser(const std::string &username, const std::string &password);

private:
    UserRepository &_userRepo;

    std::string hashPassword(const std::string &password);
};

#endif
