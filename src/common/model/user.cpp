#include "user.h"

User::User(int id, const std::string &username, const std::string &email)
    : Entity(id), _username(username), _email(email) {
}

json User::toJson() const {
    return {
        {"id", getId()},
        {"username", _username},
        {"email", _email}
    };
}

std::string User::toString() const {
    return "User: " + _username + " (ID: " + std::to_string(getId()) + ")";
}
