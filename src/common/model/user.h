#ifndef USER_H
#define USER_H

#include "entity.h"

class User : public Entity {
public:
    User() = default;

    User(int id, const std::string &username, const std::string &email);

    json toJson() const override;

    std::string toString() const override;

    // Getters/setters
    const std::string &getUsername() const { return _username; }
    const std::string &getEmail() const { return _email; }
    const std::string &getPasswordHash() const { return _passwordHash; }

    void setUsername(const std::string &username) { _username = username; }
    void setEmail(const std::string &email) { _email = email; }
    void setPasswordHash(const std::string &passwordHash) { _passwordHash = passwordHash; }

private:
    std::string _username, _email, _passwordHash;
};
#endif
