#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "sqlite_repository.h"
#include "../../common/model/model.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string>

class UserRepository : public SQLiteRepository<User> {
public:
    explicit UserRepository(std::shared_ptr<SQLite::Database> db);

    // Specialized methods
    std::optional<User> findByUsername(const std::string &username);

    std::optional<User> findByEmail(const std::string &email);

    bool validateCredentials(const std::string &username, const std::string &passwordHash);

    // Password hashing utility
    static std::string hashPassword(const std::string &password);

private:
    // Helper for loading user from query result
    static User userFromRow(SQLite::Statement &query);
};

#endif // USER_REPOSITORY_H
