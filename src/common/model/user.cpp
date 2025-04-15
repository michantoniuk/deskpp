#include "user.h"

User::User(int id, const std::string &username, const std::string &email, const std::string &fullName)
    : Entity(id), _username(username), _email(email), _fullName(fullName) {
}

json User::toJson() const {
    return {
        {"id", getId()},
        {"username", _username},
        {"email", _email},
        {"fullName", _fullName}
    };
}

std::string User::toString() const {
    return "User: " + _username + " (ID: " + std::to_string(getId()) + ")";
}
