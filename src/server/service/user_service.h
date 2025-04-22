#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "service.h"
#include "../repository/user_repository.h"

/**
 * @class UserService
 * @brief Serwis obsługujący operacje związane z użytkownikami.
 *
 * Zapewnia funkcje rejestracji, logowania i zarządzania użytkownikami.
 */
class UserService : public Service<User> {
public:
    /**
     * @brief Konstruktor
     * @param userRepository Referencja do repozytorium użytkowników
     */
    explicit UserService(UserRepository &userRepository);

    /**
     * @brief Rejestruje nowego użytkownika
     * @param username Nazwa użytkownika
     * @param password Hasło (niezaszyfrowane)
     * @param email Adres email
     * @return Obiekt JSON z wynikiem operacji
     */
    json registerUser(const std::string &username, const std::string &password, const std::string &email);

    /**
     * @brief Loguje użytkownika
     * @param username Nazwa użytkownika
     * @param password Hasło (niezaszyfrowane)
     * @return Obiekt JSON z wynikiem operacji
     */
    json loginUser(const std::string &username, const std::string &password);

private:
    UserRepository &_userRepo;

    /**
     * @brief Generuje hash hasła
     * @param password Hasło (niezaszyfrowane)
     * @return Hash hasła
     */
    std::string hashPassword(const std::string &password);
};

#endif
