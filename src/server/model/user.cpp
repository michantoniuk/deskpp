#include "user.h"

User::User() : _id(0) {
}

User::User(int id, const std::string &username, const std::string &passwordHash, 
           const std::string &email, const std::string &fullName)
    : _id(id), _username(username), _passwordHash(passwordHash), 
      _email(email), _fullName(fullName) {
}