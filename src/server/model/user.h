#ifndef SERVER_MODEL_USER_H
#define SERVER_MODEL_USER_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class User {
public:
    User();
    User(int id, const std::string &username, const std::string &passwordHash, 
         const std::string &email, const std::string &fullName);

    // Getters
    int getId() const { return _id; }
    const std::string& getUsername() const { return _username; }
    const std::string& getPasswordHash() const { return _passwordHash; }
    const std::string& getEmail() const { return _email; }
    const std::string& getFullName() const { return _fullName; }

    // Setters
    void setId(int id) { _id = id; }
    void setUsername(const std::string &username) { _username = username; }
    void setPasswordHash(const std::string &passwordHash) { _passwordHash = passwordHash; }
    void setEmail(const std::string &email) { _email = email; }
    void setFullName(const std::string &fullName) { _fullName = fullName; }

    // Convert to JSON
    json toJson() const {
        json j;
        j["id"] = _id;
        j["username"] = _username;
        j["email"] = _email;
        j["fullName"] = _fullName;
        // Don't include password hash in JSON for security
        return j;
    }

private:
    int _id;
    std::string _username;
    std::string _passwordHash; // Store hashed password, not plaintext
    std::string _email;
    std::string _fullName;
};

#endif // SERVER_MODEL_USER_H