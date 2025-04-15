#ifndef BASE_REPOSITORY_H
#define BASE_REPOSITORY_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string>
#include "../util/logger.h"

class BaseRepository {
protected:
    std::shared_ptr<SQLite::Database> _db;

    BaseRepository(std::shared_ptr<SQLite::Database> db) : _db(db) {
    }

    // Common DB operation helpers
    template<typename Func>
    auto executeQuery(const std::string &operation, Func &&func) -> decltype(func()) {
        try {
            return func();
        } catch (const SQLite::Exception &e) {
            LOG_ERROR("Error during {}: {}", operation, e.what());
            throw; // Rethrow to be handled by the caller
        }
    }
};

#endif // BASE_REPOSITORY_H
