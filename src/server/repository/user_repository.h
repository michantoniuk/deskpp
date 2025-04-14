#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <vector>
#include <string>
#include <optional>
#include "../db/database.h"
#include "../model/user.h"

class UserRepository {
public:
    explicit UserRepository(Database &db);

    std::vector<User> getAllUsers();
    std::optional<User> getUserById(int id);
    std::optional<User> getUserByUsername(const std::string &username);
    std::optional<User> getUserByEmail(const std::string &email);
    
    // Returns ID of the newly created user, or nullopt if failed
    std::optional<int64_t> createUser(const std::string &username, 
                                     const std::string &passwordHash,
                                     const std::string &email,
                                     const std::string &fullName);
    
    bool updateUser(int id, const User &user);
    bool deleteUser(int id);
    
    // Authentication
    bool validateCredentials(const std::string &username, const std::string &passwordHash);
    
private:
    Database &_db;
    
    User mapRowToUser(const std::map<std::string, std::string> &row);
};

#endif // USER_REPOSITORY_H