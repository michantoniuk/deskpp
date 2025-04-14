#include "user.h"

User::User() : _id(0) {
}

User::User(int id, const std::string &username, const std::string &email, const std::string &fullName)
    : _id(id), _username(username), _email(email), _fullName(fullName) {
}

std::string User::toJson() const {
    json j;
    j["id"] = _id;
    j["username"] = _username;
    j["email"] = _email;
    j["fullName"] = _fullName;

    return j.dump();
}

User User::fromJson(const std::string& jsonStr) {
    json j;
    try {
        j = json::parse(jsonStr);
    } catch (...) {
        // Return default user if parsing fails
        return User();
    }

    User user;

    // Parse fields
    if (j.contains("id") && !j["id"].is_null()) {
        user.setId(j["id"].get<int>());
    }

    if (j.contains("username") && !j["username"].is_null()) {
        user.setUsername(j["username"].get<std::string>());
    }

    if (j.contains("email") && !j["email"].is_null()) {
        user.setEmail(j["email"].get<std::string>());
    }

    if (j.contains("fullName") && !j["fullName"].is_null()) {
        user.setFullName(j["fullName"].get<std::string>());
    }

    return user;
}