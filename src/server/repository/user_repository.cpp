#include "user_repository.h"
#include <functional>
#include "../util/logger.h"

UserRepository::UserRepository(std::shared_ptr<SQLite::Database> db)
    : BaseRepository(db) {
}

std::vector<User> UserRepository::findAll() {
    std::vector<User> users;

    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users ORDER BY username");

        while (query.executeStep()) {
            User user(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            user.setPasswordHash(query.getColumn(2).getString());
            users.push_back(user);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting users: {}", e.what());
    }

    return users;
}

std::optional<User> UserRepository::findById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            User user(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            user.setPasswordHash(query.getColumn(2).getString());
            return user;
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting user by ID: {}", e.what());
    }

    return std::nullopt;
}

User UserRepository::add(const User &user) {
    try {
        SQLite::Statement query(*_db, "INSERT INTO users (username, password_hash, email, full_name) "
                                "VALUES (?, ?, ?, ?)");
        query.bind(1, user.getUsername());
        query.bind(2, user.getPasswordHash());
        query.bind(3, user.getEmail());
        query.bind(4, user.getFullName());

        query.exec();
        int id = static_cast<int>(_db->getLastInsertRowid());

        User newUser = user;
        newUser.setId(id);
        return newUser;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error creating user: {}", e.what());
        return user; // Return original user with ID 0 to indicate failure
    }
}

bool UserRepository::update(const User &user) {
    try {
        SQLite::Statement query(*_db, "UPDATE users SET username = ?, password_hash = ?, "
                                "email = ?, full_name = ? WHERE id = ?");
        query.bind(1, user.getUsername());
        query.bind(2, user.getPasswordHash());
        query.bind(3, user.getEmail());
        query.bind(4, user.getFullName());
        query.bind(5, user.getId());

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error updating user: {}", e.what());
        return false;
    }
}

bool UserRepository::remove(int id) {
    try {
        SQLite::Statement query(*_db, "DELETE FROM users WHERE id = ?");
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error deleting user: {}", e.what());
        return false;
    }
}

std::optional<User> UserRepository::findByUsername(const std::string &username) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE username = ?");
        query.bind(1, username);

        if (query.executeStep()) {
            User user(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            user.setPasswordHash(query.getColumn(2).getString());
            return user;
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
            User user(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            user.setPasswordHash(query.getColumn(2).getString());
            return user;
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
