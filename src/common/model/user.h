#ifndef USER_H
#define USER_H

#include "entity.h"

/**
 * @class User
 * @brief Klasa reprezentująca użytkownika systemu.
 *
 * Przechowuje podstawowe informacje o użytkowniku takie jak
 * nazwa użytkownika, email i hash hasła.
 */
class User : public Entity {
public:
    /**
     * @brief Konstruktor domyślny
     */
    User() = default;

    /**
     * @brief Konstruktor z parametrami
     * @param id Identyfikator użytkownika
     * @param username Nazwa użytkownika
     * @param email Adres email
     */
    User(int id, const std::string &username, const std::string &email);

    /**
     * @brief Konwertuje obiekt na format JSON
     * @return Reprezentacja JSON obiektu
     */
    json toJson() const override;

    /**
     * @brief Konwertuje obiekt na string
     * @return Tekstowa reprezentacja obiektu
     */
    std::string toString() const override;

    /**
     * @brief Pobiera nazwę użytkownika
     * @return Nazwa użytkownika
     */
    const std::string &getUsername() const { return _username; }

    /**
     * @brief Pobiera email użytkownika
     * @return Adres email
     */
    const std::string &getEmail() const { return _email; }

    /**
     * @brief Pobiera hash hasła użytkownika
     * @return Hash hasła
     */
    const std::string &getPasswordHash() const { return _passwordHash; }

    /**
     * @brief Ustawia nazwę użytkownika
     * @param username Nowa nazwa użytkownika
     */
    void setUsername(const std::string &username) { _username = username; }

    /**
     * @brief Ustawia email użytkownika
     * @param email Nowy adres email
     */
    void setEmail(const std::string &email) { _email = email; }

    /**
     * @brief Ustawia hash hasła użytkownika
     * @param passwordHash Nowy hash hasła
     */
    void setPasswordHash(const std::string &passwordHash) { _passwordHash = passwordHash; }

private:
    std::string _username, _email, _passwordHash;
};
#endif
