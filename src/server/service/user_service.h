#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "service.h"
#include "../repository/user_repository.h"

class UserService : public Service<User> {
public:
    explicit UserService(UserRepository &userRepository);

    json getAllUsers();

    json getUserById(int id);

    json registerUser(const std::string &username, const std::string &password,
                      const std::string &email, const std::string &fullName);

    json loginUser(const std::string &username, const std::string &password);

    json updateUser(int id, const json &userData);

    json deleteUser(int id);

private:
    UserRepository &_userRepo;

    std::string hashPassword(const std::string &password);
};

#endif // USER_SERVICE_H
