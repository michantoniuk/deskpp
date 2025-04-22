#include "user_service.h"

UserService::UserService(UserRepository &userRepository)
    : Service<User>(userRepository), _userRepo(userRepository) {
}

json UserService::registerUser(const std::string &username, const std::string &password,
                               const std::string &email) {
    // Sprawdź czy użytkownik istnieje
    if (_userRepo.findByUsername(username) || _userRepo.findByEmail(email)) {
        return errorResponse("Nazwa użytkownika lub email już istnieje");
    }

    // Utwórz użytkownika
    User user;
    user.setUsername(username);
    user.setEmail(email);
    user.setPasswordHash(hashPassword(password));

    User createdUser = _repository.add(user);
    return successResponse({{"user", createdUser.toJson()}});
}

json UserService::loginUser(const std::string &username, const std::string &password) {
    // Generuj hash hasła
    std::string passwordHash = hashPassword(password);

    // Sprawdź dane logowania
    if (!_userRepo.validateCredentials(username, passwordHash)) {
        return errorResponse("Nieprawidłowa nazwa użytkownika lub hasło");
    }

    // Pobierz użytkownika
    auto user = _userRepo.findByUsername(username);
    if (!user) {
        return errorResponse("Nie znaleziono użytkownika");
    }

    return successResponse({{"user", user->toJson()}});
}

std::string UserService::hashPassword(const std::string &password) {
    return UserRepository::hashPassword(password);
}
