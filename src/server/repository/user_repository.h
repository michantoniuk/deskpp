#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/user.h"
#include <memory>

/**
 * @class UserRepository
 * @brief Repozytorium do zarządzania użytkownikami w bazie danych.
 *
 * Zapewnia operacje CRUD na użytkownikach oraz dodatkowe funkcje
 * do wyszukiwania i weryfikacji użytkowników.
 */
class UserRepository : public SQLiteRepository<User> {
public:
    /**
     * @brief Konstruktor
     * @param db Współdzielony wskaźnik do bazy danych
     */
    explicit UserRepository(std::shared_ptr<SQLite::Database> db);

    /**
     * @brief Wyszukuje użytkownika po nazwie
     * @param username Nazwa użytkownika
     * @return Opcjonalny obiekt użytkownika (brak w przypadku nieznalezienia)
     */
    std::optional<User> findByUsername(const std::string &username);

    /**
     * @brief Wyszukuje użytkownika po adresie email
     * @param email Adres email
     * @return Opcjonalny obiekt użytkownika (brak w przypadku nieznalezienia)
     */
    std::optional<User> findByEmail(const std::string &email);

    /**
     * @brief Weryfikuje dane logowania użytkownika
     * @param username Nazwa użytkownika
     * @param passwordHash Hash hasła
     * @return Czy dane logowania są poprawne
     */
    bool validateCredentials(const std::string &username, const std::string &passwordHash);

    /**
     * @brief Generuje hash hasła
     * @param password Hasło (niezaszyfrowane)
     * @return Hash hasła
     */
    static std::string hashPassword(const std::string &password);

private:
    /**
     * @brief Konwertuje wiersz z bazy na obiekt użytkownika
     * @param query Zapytanie SQL z wynikami
     * @return Obiekt użytkownika
     */
    static User userFromRow(SQLite::Statement &query);
};

#endif
