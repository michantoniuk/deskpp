#include "user_repository.h"
#include <functional>
#include "common/logger.h"

UserRepository::UserRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<User>(
        db,
        "users",
        "SELECT id, username, password_hash, email, full_name FROM users ORDER BY username",
        "SELECT id, username, password_hash, email, full_name FROM users WHERE id = ?",
        "INSERT INTO users (username, password_hash, email, full_name) VALUES (?, ?, ?, ?)",
        "UPDATE users SET username = ?, password_hash = ?, email = ?, full_name = ? WHERE id = ?",
        "DELETE FROM users WHERE id = ?",
        userFromRow,
        [](SQLite::Statement &stmt, const User &user) {
            stmt.bind(1, user.getUsername());
            stmt.bind(2, user.getPasswordHash());
            stmt.bind(3, user.getEmail());
            stmt.bind(4, user.getFullName());
        }
    ) {
}

User UserRepository::userFromRow(SQLite::Statement &query) {
    User user(
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(3).getString(),
        query.getColumn(4).getString()
    );
    user.setPasswordHash(query.getColumn(2).getString());
    return user;
}

std::optional<User> UserRepository::findByUsername(const std::string &username) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE username = ?");
        query.bind(1, username);

        if (query.executeStep()) {
            return userFromRow(query);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting user by username: {}", e.what());
    }

    return std::nullopt;
}

std::optional<User> UserRepository::findByEmail(const std::string &email) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE email = ?");
        query.bind(1, email);

        if (query.executeStep()) {
            return userFromRow(query);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting user by email: {}", e.what());
    }

    return std::nullopt;
}

bool UserRepository::validateCredentials(const std::string &username, const std::string &passwordHash) {
    try {
        SQLite::Statement query(*_db, "SELECT COUNT(*) FROM users "
                                "WHERE username = ? AND password_hash = ?");
        query.bind(1, username);
        query.bind(2, passwordHash);

        if (query.executeStep()) {
            return query.getColumn(0).getInt() > 0;
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error validating credentials: {}", e.what());
    }

    return false;
}

std::string UserRepository::hashPassword(const std::string &password) {
    // Simple hashing for demonstration purposes
    // In a real app, use a proper hashing library with salt
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    return std::to_string(hash);
}
