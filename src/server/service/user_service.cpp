#include "user_service.h"
#include "../util/logger.h"
#include <regex>
#include <functional> // for std::hash

UserService::UserService(UserRepository &userRepo) : _userRepo(userRepo) {
}

json UserService::getAllUsers() {
    auto users = _userRepo.getAllUsers();

    json usersArray = json::array();
    for (const auto &user : users) {
        usersArray.push_back(user.toJson());
    }

    return {{"status", "success"}, {"users", usersArray}};
}

json UserService::getUserById(int id) {
    auto user = _userRepo.getUserById(id);
    
    if (!user) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    return {{"status", "success"}, {"user", user->toJson()}};
}

json UserService::getUserByUsername(const std::string &username) {
    auto user = _userRepo.getUserByUsername(username);
    
    if (!user) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    return {{"status", "success"}, {"user", user->toJson()}};
}

json UserService::registerUser(const std::string &username, 
                             const std::string &password,
                             const std::string &email,
                             const std::string &fullName) {
    // Validate input data
    if (!validateUserData(username, password, email)) {
        return {{"status", "error"}, {"message", "Invalid user data"}};
    }
    
    // Hash the password
    std::string passwordHash = hashPassword(password);
    
    // Create user
    auto userId = _userRepo.createUser(username, passwordHash, email, fullName);
    if (!userId) {
        return {{"status", "error"}, {"message", "Username or email already exists"}};
    }
    
    // Get the created user
    auto user = _userRepo.getUserById(*userId);
    if (!user) {
        return {{"status", "error"}, {"message", "Error retrieving user data"}};
    }
    
    return {
        {"status", "success"},
        {"message", "User registered successfully"},
        {"user", user->toJson()}
    };
}

json UserService::loginUser(const std::string &username, const std::string &password) {
    // Hash the password
    std::string passwordHash = hashPassword(password);
    
    // Validate credentials
    bool isValid = _userRepo.validateCredentials(username, passwordHash);
    if (!isValid) {
        return {{"status", "error"}, {"message", "Invalid username or password"}};
    }
    
    // Get user data
    auto user = _userRepo.getUserByUsername(username);
    if (!user) {
        return {{"status", "error"}, {"message", "Error retrieving user data"}};
    }
    
    return {
        {"status", "success"},
        {"message", "Login successful"},
        {"user", user->toJson()}
    };
}

json UserService::updateUser(int id, const json &userData) {
    // Get existing user
    auto existingUser = _userRepo.getUserById(id);
    if (!existingUser) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    User updatedUser = *existingUser;
    
    // Update user fields if provided
    if (userData.contains("username") && !userData["username"].is_null()) {
        updatedUser.setUsername(userData["username"].get<std::string>());
    }
    
    if (userData.contains("password") && !userData["password"].is_null()) {
        std::string passwordHash = hashPassword(userData["password"].get<std::string>());
        updatedUser.setPasswordHash(passwordHash);
    }
    
    if (userData.contains("email") && !userData["email"].is_null()) {
        updatedUser.setEmail(userData["email"].get<std::string>());
    }
    
    if (userData.contains("fullName") && !userData["fullName"].is_null()) {
        updatedUser.setFullName(userData["fullName"].get<std::string>());
    }
    
    // Update user in repository
    bool success = _userRepo.updateUser(id, updatedUser);
    if (!success) {
        return {{"status", "error"}, {"message", "Error updating user"}};
    }
    
    return {
        {"status", "success"},
        {"message", "User updated successfully"},
        {"user", updatedUser.toJson()}
    };
}

json UserService::deleteUser(int id) {
    // Check if user exists
    auto user = _userRepo.getUserById(id);
    if (!user) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    // Delete user
    bool success = _userRepo.deleteUser(id);
    if (!success) {
        return {{"status", "error"}, {"message", "Error deleting user"}};
    }
    
    return {
        {"status", "success"},
        {"message", "User deleted successfully"}
    };
}

std::string UserService::hashPassword(const std::string &password) {
    // This is a very simple hash for demonstration purposes
    // In a production environment, use a proper hashing library with salt
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    return std::to_string(hash);
}

bool UserService::validateUserData(const std::string &username, 
                                 const std::string &password,
                                 const std::string &email) {
    // Check if username is valid (alphanumeric, 3-20 chars)
    std::regex usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    if (!std::regex_match(username, usernameRegex)) {
        return false;
    }
    
    // Check if password is valid (at least 6 chars)
    if (password.length() < 6) {
        return false;
    }
    
    // Check if email is valid
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex)) {
        return false;
    }
    
    return true;
}