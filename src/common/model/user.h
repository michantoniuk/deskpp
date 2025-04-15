#ifndef USER_H
#define USER_H

#include "entity.h"
#include <string>

// User model
class User : public Entity {
public:
    User() = default;

    User(int id, const std::string &username, const std::string &email, const std::string &fullName);

    // From Entity
    json toJson() const override;

    std::string toString() const override;

    // Getters/setters
    const std::string &getUsername() const { return _username; }
    const std::string &getEmail() const { return _email; }
    const std::string &getFullName() const { return _fullName; }
    const std::string &getPasswordHash() const { return _passwordHash; }

    void setUsername(const std::string &username) { _username = username; }
    void setEmail(const std::string &email) { _email = email; }
    void setFullName(const std::string &fullName) { _fullName = fullName; }
    void setPasswordHash(const std::string &passwordHash) { _passwordHash = passwordHash; }

private:
    std::string _username;
    std::string _email;
    std::string _fullName;
    std::string _passwordHash;
};

#endif // USER_H
