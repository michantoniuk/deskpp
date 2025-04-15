#include "user_service.h"
#include <regex>
#include <functional>
#include "../util/logger.h"

UserService::UserService(UserRepository &userRepository)
    : _userRepository(userRepository) {
}

json UserService::getAllUsers() {
    auto users = _userRepository.findAll();
    return entityListToJson(users, "users");
}

json UserService::getUserById(int id) {
    auto user = _userRepository.findById(id);

    if (!user) {
        return errorResponse("User not found");
    }

    return successResponse({{"user", user->toJson()}});
}

json UserService::getUserByUsername(const std::string &username) {
    auto user = _userRepository.findByUsername(username);

    if (!user) {
        return errorResponse("User not found");
    }

    return successResponse({{"user", user->toJson()}});
}

json UserService::registerUser(const std::string &username,
                               const std::string &password,
                               const std::string &email,
                               const std::string &fullName) {
    // Validate input data
    if (!validateUserData(username, password, email)) {
        return errorResponse("Invalid user data");
    }

    // Check if user already exists
    if (_userRepository.findByUsername(username) || _userRepository.findByEmail(email)) {
        return errorResponse("Username or email already exists");
    }

    // Create user object
    User user;
    user.setUsername(username);
    user.setEmail(email);
    user.setFullName(fullName);
    user.setPasswordHash(hashPassword(password));

    // Create user in repository
    User createdUser = _userRepository.add(user);

    // Check if user was created successfully
    if (createdUser.getId() <= 0) {
        return errorResponse("Error registering user");
    }

    return successResponse({
        {"message", "User registered successfully"},
        {"user", createdUser.toJson()}
    });
}

json UserService::loginUser(const std::string &username, const std::string &password) {
    // Hash the password
    std::string passwordHash = hashPassword(password);

    // Validate credentials
    bool isValid = _userRepository.validateCredentials(username, passwordHash);
    if (!isValid) {
        return errorResponse("Invalid username or password");
    }

    // Get user data
    auto user = _userRepository.findByUsername(username);
    if (!user) {
        return errorResponse("Error retrieving user data");
    }

    return successResponse({
        {"message", "Login successful"},
        {"user", user->toJson()}
    });
}

json UserService::updateUser(int id, const json &userData) {
    // Get existing user
    auto existingUser = _userRepository.findById(id);
    if (!existingUser) {
        return errorResponse("User not found");
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
    bool success = _userRepository.update(updatedUser);
    if (!success) {
        return errorResponse("Error updating user");
    }

    return successResponse({
        {"message", "User updated successfully"},
        {"user", updatedUser.toJson()}
    });
}

json UserService::deleteUser(int id) {
    // Check if user exists
    auto user = _userRepository.findById(id);
    if (!user) {
        return errorResponse("User not found");
    }

    // Delete user
    bool success = _userRepository.remove(id);
    if (!success) {
        return errorResponse("Error deleting user");
    }

    return successResponse({
        {"message", "User deleted successfully"}
    });
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

std::string UserService::hashPassword(const std::string &password) {
    // Simple hashing for demonstration purposes
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    return std::to_string(hash);
}
