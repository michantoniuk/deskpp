#include "user_service.h"
#include <regex>
#include "common/logger.h"

UserService::UserService(UserRepository &userRepository)
    : Service<User>(userRepository), _userRepo(userRepository) {
}

json UserService::getAllUsers() {
    return getAll("users");
}

json UserService::getUserById(int id) {
    return getById(id, "user", "User not found");
}

json UserService::getUserByUsername(const std::string &username) {
    auto user = _userRepo.findByUsername(username);

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
    if (_userRepo.findByUsername(username) || _userRepo.findByEmail(email)) {
        return errorResponse("Username or email already exists");
    }

    // Create user object
    User user;
    user.setUsername(username);
    user.setEmail(email);
    user.setFullName(fullName);
    user.setPasswordHash(hashPassword(password));

    // Create user in repository
    User createdUser = _repository.add(user);

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
    bool isValid = _userRepo.validateCredentials(username, passwordHash);
    if (!isValid) {
        return errorResponse("Invalid username or password");
    }

    // Get user data
    auto user = _userRepo.findByUsername(username);
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
    auto existingUser = _repository.findById(id);
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
    bool success = _repository.update(updatedUser);
    if (!success) {
        return errorResponse("Error updating user");
    }

    return successResponse({
        {"message", "User updated successfully"},
        {"user", updatedUser.toJson()}
    });
}

json UserService::deleteUser(int id) {
    return removeById(id, "User deleted successfully", "User not found");
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
    return UserRepository::hashPassword(password);
}
