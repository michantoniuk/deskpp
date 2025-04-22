#include "user_repository.h"
#include <functional>

UserRepository::UserRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<User>(
        db,
        "users",
        "SELECT id, username, password_hash, email FROM users ORDER BY username",
        "SELECT id, username, password_hash, email FROM users WHERE id = ?",
        "INSERT INTO users (username, password_hash, email) VALUES (?, ?, ?)",
        "UPDATE users SET username = ?, password_hash = ?, email = ? WHERE id = ?",
        "DELETE FROM users WHERE id = ?",
        userFromRow,
        [](SQLite::Statement &stmt, const User &user) {
            stmt.bind(1, user.getUsername());
            stmt.bind(2, user.getPasswordHash());
            stmt.bind(3, user.getEmail());
        }
    ) {
}

User UserRepository::userFromRow(SQLite::Statement &query) {
    User user(
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(3).getString()
    );
    user.setPasswordHash(query.getColumn(2).getString());
    return user;
}

std::optional<User> UserRepository::findByUsername(const std::string &username) {
    SQLite::Statement query(*_db, "SELECT id, username, password_hash, email "
                            "FROM users WHERE username = ?");
    query.bind(1, username);

    if (query.executeStep()) {
        return userFromRow(query);
    }
    return std::nullopt;
}

std::optional<User> UserRepository::findByEmail(const std::string &email) {
    SQLite::Statement query(*_db, "SELECT id, username, password_hash, email "
                            "FROM users WHERE email = ?");
    query.bind(1, email);

    if (query.executeStep()) {
        return userFromRow(query);
    }
    return std::nullopt;
}

bool UserRepository::validateCredentials(const std::string &username, const std::string &passwordHash) {
    SQLite::Statement query(*_db, "SELECT COUNT(*) FROM users "
                            "WHERE username = ? AND password_hash = ?");
    query.bind(1, username);
    query.bind(2, passwordHash);

    if (query.executeStep()) {
        return query.getColumn(0).getInt() > 0;
    }
    return false;
}

std::string UserRepository::hashPassword(const std::string &password) {
    // Simple hashing for demonstration
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}
