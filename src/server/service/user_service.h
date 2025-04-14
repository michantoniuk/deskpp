#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include <string>
#include <nlohmann/json.hpp>
#include "../repository/user_repository.h"

using json = nlohmann::json;

class UserService {
public:
    explicit UserService(UserRepository &userRepo);

    json getAllUsers();
    json getUserById(int id);
    json getUserByUsername(const std::string &username);
    
    json registerUser(const std::string &username, 
                     const std::string &password,
                     const std::string &email,
                     const std::string &fullName);
    
    json loginUser(const std::string &username, const std::string &password);
    json updateUser(int id, const json &userData);
    json deleteUser(int id);

private:
    UserRepository &_userRepo;
    
    // Simple password hashing function - in production, use a proper library
    std::string hashPassword(const std::string &password);
    
    // Validates user data
    bool validateUserData(const std::string &username, 
                         const std::string &password,
                         const std::string &email);
};

#endif // USER_SERVICE_H