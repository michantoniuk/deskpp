#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "controller.h"
#include "../../service/user_service.h"

/**
 * @class UserController
 * @brief Kontroler do zarządzania użytkownikami.
 *
 * Obsługuje żądania HTTP związane z użytkownikami,
 * takimi jak rejestracja i logowanie.
 */
class UserController : public Controller {
public:
    /**
     * @brief Konstruktor
     * @param userService Referencja do serwisu użytkowników
     */
    explicit UserController(UserService &userService);

    /**
     * @brief Obsługuje żądanie rejestracji użytkownika
     * @param req Żądanie HTTP
     * @return Odpowiedź HTTP z wynikiem operacji
     */
    crow::response registerUser(const crow::request &req);

    /**
     * @brief Obsługuje żądanie logowania użytkownika
     * @param req Żądanie HTTP
     * @return Odpowiedź HTTP z wynikiem operacji
     */
    crow::response loginUser(const crow::request &req);

private:
    UserService &_userService;
};

#endif
