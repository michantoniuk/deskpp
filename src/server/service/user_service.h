#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "base_service.h"
#include <string>
#include <nlohmann/json.hpp>
#include "../repository/user_repository.h"

using json = nlohmann::json;

/**
 * Service layer for user operations
 */
class UserService : public BaseService {
public:
    explicit UserService(UserRepository &userRepository);

    json getAllUsers();

    json getUserById(int id);

    json getUserByUsername(const std::string &username);

    json registerUser(const std::string &username, const std::string &password,
                      const std::string &email, const std::string &fullName);

    json loginUser(const std::string &username, const std::string &password);

    json updateUser(int id, const json &userData);

    json deleteUser(int id);

private:
    UserRepository &_userRepository;

    // Helpers
    bool validateUserData(const std::string &username, const std::string &password,
                          const std::string &email);

    std::string hashPassword(const std::string &password);
};

#endif // USER_SERVICE_H
