#include "user_service.h"

UserService::UserService(UserRepository &userRepository)
    : Service<User>(userRepository), _userRepo(userRepository) {
}

json UserService::registerUser(const std::string &username, const std::string &password,
                               const std::string &email) {
    // Check if user exists
    if (_userRepo.findByUsername(username) || _userRepo.findByEmail(email)) {
        return errorResponse("Username or email already exists");
    }

    // Create user
    User user;
    user.setUsername(username);
    user.setEmail(email);
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

std::string UserService::hashPassword(const std::string &password) {
    return UserRepository::hashPassword(password);
}
