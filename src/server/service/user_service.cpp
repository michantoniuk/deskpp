#include "user_service.h"

UserService::UserService(UserRepository &userRepository)
    : Service<User>(userRepository), _userRepo(userRepository) {
}

json UserService::getAllUsers() {
    auto users = _repository.findAll();
    return entityListToJson(users, "users");
}

json UserService::getUserById(int id) {
    auto user = _repository.findById(id);
    if (!user) {
        return errorResponse("User not found");
    }
    return successResponse({{"user", user->toJson()}});
}

json UserService::registerUser(const std::string &username, const std::string &password,
                               const std::string &email, const std::string &fullName) {
    // Check if user exists
    if (_userRepo.findByUsername(username) || _userRepo.findByEmail(email)) {
        return errorResponse("Username or email already exists");
    }

    // Create user
    User user;
    user.setUsername(username);
    user.setEmail(email);
    user.setFullName(fullName);
    user.setPasswordHash(hashPassword(password));

    User createdUser = _repository.add(user);
    return successResponse({{"user", createdUser.toJson()}});
}

json UserService::loginUser(const std::string &username, const std::string &password) {
    // Hash password
    std::string passwordHash = hashPassword(password);

    // Check credentials
    if (!_userRepo.validateCredentials(username, passwordHash)) {
        return errorResponse("Invalid username or password");
    }

    // Get user
    auto user = _userRepo.findByUsername(username);
    if (!user) {
        return errorResponse("User not found");
    }

    return successResponse({{"user", user->toJson()}});
}

json UserService::updateUser(int id, const json &userData) {
    // Get user
    auto user = _repository.findById(id);
    if (!user) {
        return errorResponse("User not found");
    }

    // Update fields
    if (userData.contains("username")) {
        user->setUsername(userData["username"].get<std::string>());
    }
    if (userData.contains("email")) {
        user->setEmail(userData["email"].get<std::string>());
    }
    if (userData.contains("fullName")) {
        user->setFullName(userData["fullName"].get<std::string>());
    }
    if (userData.contains("password")) {
        user->setPasswordHash(hashPassword(userData["password"].get<std::string>()));
    }

    // Save changes
    _repository.update(*user);
    return successResponse({{"user", user->toJson()}});
}

json UserService::deleteUser(int id) {
    auto user = _repository.findById(id);
    if (!user) {
        return errorResponse("User not found");
    }

    _repository.remove(id);
    return successResponse({{"message", "User deleted"}});
}

std::string UserService::hashPassword(const std::string &password) {
    return UserRepository::hashPassword(password);
}
