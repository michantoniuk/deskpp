#ifndef CLIENT_MODEL_USER_H
#define CLIENT_MODEL_USER_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class User {
public:
    User();
    User(int id, const std::string &username, const std::string &email, const std::string &fullName);

    std::string toJson() const;
    static User fromJson(const std::string& jsonStr);

    // Getters
    int getId() const { return _id; }
    std::string getUsername() const { return _username; }
    std::string getEmail() const { return _email; }
    std::string getFullName() const { return _fullName; }

    // Setters
    void setId(int id) { _id = id; }
    void setUsername(const std::string &username) { _username = username; }
    void setEmail(const std::string &email) { _email = email; }
    void setFullName(const std::string &fullName) { _fullName = fullName; }

private:
    int _id;
    std::string _username;
    std::string _email;
    std::string _fullName;
};

#endif // CLIENT_MODEL_USER_H