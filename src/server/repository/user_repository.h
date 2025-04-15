#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "common/repository.h"
#include "base_repository.h"
#include "common/models.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>

class UserRepository : public Repository<User>, public BaseRepository {
public:
    explicit UserRepository(std::shared_ptr<SQLite::Database> db);

    std::vector<User> findAll() override;

    std::optional<User> findById(int id) override;

    User add(const User &user) override;

    bool update(const User &user) override;

    bool remove(int id) override;

    // Additional specialized methods
    std::optional<User> findByUsername(const std::string &username);

    std::optional<User> findByEmail(const std::string &email);

    bool validateCredentials(const std::string &username, const std::string &passwordHash);

private:
    // Helper methods
    std::string hashPassword(const std::string &password);
};

#endif // USER_REPOSITORY_H
