#include "user_repository.h"
#include "../util/logger.h"

UserRepository::UserRepository(Database& db) : _db(db) {
}

std::vector<User> UserRepository::getAllUsers() {
    std::vector<User> users;

    std::string sql = "SELECT id, username, password_hash, email, full_name FROM users ORDER BY username";

    _db.query(sql, [this, &users](auto row) {
        users.push_back(this->mapRowToUser(row));
    });

    return users;
}

std::optional<User> UserRepository::getUserById(int id) {
    std::optional<User> user;

    std::string sql = "SELECT id, username, password_hash, email, full_name FROM users WHERE id = " + 
                      std::to_string(id);

    _db.query(sql, [this, &user](auto row) {
        if (!user) {
            user = this->mapRowToUser(row);
        }
    });

    return user;
}

std::optional<User> UserRepository::getUserByUsername(const std::string& username) {
    std::optional<User> user;

    std::string sql = "SELECT id, username, password_hash, email, full_name FROM users WHERE username = '" + 
                      username + "'";

    _db.query(sql, [this, &user](auto row) {
        if (!user) {
            user = this->mapRowToUser(row);
        }
    });

    return user;
}

std::optional<User> UserRepository::getUserByEmail(const std::string& email) {
    std::optional<User> user;

    std::string sql = "SELECT id, username, password_hash, email, full_name FROM users WHERE email = '" + 
                      email + "'";

    _db.query(sql, [this, &user](auto row) {
        if (!user) {
            user = this->mapRowToUser(row);
        }
    });

    return user;
}

std::optional<int64_t> UserRepository::createUser(const std::string& username, 
                                                 const std::string& passwordHash,
                                                 const std::string& email,
                                                 const std::string& fullName) {
    // Check if username or email already exists
    auto existingUser = getUserByUsername(username);
    if (existingUser) {
        LOG_ERROR("Username already exists: {}", username);
        return std::nullopt;
    }
    
    existingUser = getUserByEmail(email);
    if (existingUser) {
        LOG_ERROR("Email already exists: {}", email);
        return std::nullopt;
    }
    
    std::string sql = "INSERT INTO users (username, password_hash, email, full_name) VALUES ('" +
                     username + "', '" +
                     passwordHash + "', '" +
                     email + "', '" +
                     fullName + "')";

    if (!_db.execute(sql)) {
        LOG_ERROR("Failed to create user: {}", username);
        return std::nullopt;
    }

    return _db.lastInsertId();
}

bool UserRepository::updateUser(int id, const User& user) {
    std::string sql = "UPDATE users SET "
                     "username = '" + user.getUsername() + "', " +
                     "password_hash = '" + user.getPasswordHash() + "', " +
                     "email = '" + user.getEmail() + "', " +
                     "full_name = '" + user.getFullName() + "' " +
                     "WHERE id = " + std::to_string(id);

    if (!_db.execute(sql)) {
        LOG_ERROR("Failed to update user: {}", id);
        return false;
    }

    return true;
}

bool UserRepository::deleteUser(int id) {
    std::string sql = "DELETE FROM users WHERE id = " + std::to_string(id);

    if (!_db.execute(sql)) {
        LOG_ERROR("Failed to delete user: {}", id);
        return false;
    }

    return true;
}

bool UserRepository::validateCredentials(const std::string& username, const std::string& passwordHash) {
    bool isValid = false;

    std::string sql = "SELECT COUNT(*) as count FROM users WHERE username = '" + username + 
                     "' AND password_hash = '" + passwordHash + "'";

    _db.query(sql, [&isValid](auto row) {
        isValid = (std::stoi(row["count"]) > 0);
    });

    return isValid;
}

User UserRepository::mapRowToUser(const std::map<std::string, std::string>& row) {
    int id = std::stoi(row.at("id"));
    std::string username = row.at("username");
    std::string passwordHash = row.at("password_hash");
    std::string email = row.at("email");
    std::string fullName = row.at("full_name");

    return User(id, username, passwordHash, email, fullName);
}