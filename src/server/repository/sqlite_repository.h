#ifndef SQLITE_REPOSITORY_H
#define SQLITE_REPOSITORY_H

#include "repository.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string>
#include "common/logger.h"

template<typename T>
class SQLiteRepository : public Repository<T> {
protected:
    std::shared_ptr<SQLite::Database> _db;
    std::string _tableName;

    // SQL query templates
    std::string _findAllQuery;
    std::string _findByIdQuery;
    std::string _insertQuery;
    std::string _updateQuery;
    std::string _deleteQuery;

    // Function to convert SQLite query result to entity
    std::function<T(SQLite::Statement &)> _rowToEntity;

    // Function to bind entity to insert/update statement
    std::function<void(SQLite::Statement &, const T &)> _bindEntity;

public:
    SQLiteRepository(
        std::shared_ptr<SQLite::Database> db,
        const std::string &tableName,
        const std::string &findAllQuery,
        const std::string &findByIdQuery,
        const std::string &insertQuery,
        const std::string &updateQuery,
        const std::string &deleteQuery,
        std::function<T(SQLite::Statement &)> rowToEntity,
        std::function<void(SQLite::Statement &, const T &)> bindEntity
    );

    // Implementation of Repository interface
    std::vector<T> findAll() override;

    std::optional<T> findById(int id) override;

    T add(const T &entity) override;

    bool update(const T &entity) override;

    bool remove(int id) override;

    // Helper for executing custom queries
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

// Template implementation needs to be in the header
// Constructor implementation
template<typename T>
SQLiteRepository<T>::SQLiteRepository(
    std::shared_ptr<SQLite::Database> db,
    const std::string &tableName,
    const std::string &findAllQuery,
    const std::string &findByIdQuery,
    const std::string &insertQuery,
    const std::string &updateQuery,
    const std::string &deleteQuery,
    std::function<T(SQLite::Statement &)> rowToEntity,
    std::function<void(SQLite::Statement &, const T &)> bindEntity
) : _db(db),
    _tableName(tableName),
    _findAllQuery(findAllQuery),
    _findByIdQuery(findByIdQuery),
    _insertQuery(insertQuery),
    _updateQuery(updateQuery),
    _deleteQuery(deleteQuery),
    _rowToEntity(rowToEntity),
    _bindEntity(bindEntity) {
}

// findAll implementation
template<typename T>
std::vector<T> SQLiteRepository<T>::findAll() {
    std::vector<T> entities;

    try {
        SQLite::Statement query(*_db, _findAllQuery);

        while (query.executeStep()) {
            entities.push_back(_rowToEntity(query));
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error finding all entities from {}: {}", _tableName, e.what());
    }

    return entities;
}

// findById implementation
template<typename T>
std::optional<T> SQLiteRepository<T>::findById(int id) {
    try {
        SQLite::Statement query(*_db, _findByIdQuery);
        query.bind(1, id);

        if (query.executeStep()) {
            return _rowToEntity(query);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error finding entity by id from {}: {}", _tableName, e.what());
    }

    return std::nullopt;
}

// add implementation
template<typename T>
T SQLiteRepository<T>::add(const T &entity) {
    try {
        SQLite::Statement query(*_db, _insertQuery);
        _bindEntity(query, entity);

        query.exec();
        int id = static_cast<int>(_db->getLastInsertRowid());

        T newEntity = entity;
        newEntity.setId(id);
        return newEntity;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error adding entity to {}: {}", _tableName, e.what());
        return entity; // Return original entity with ID 0 to indicate failure
    }
}

// update implementation
template<typename T>
bool SQLiteRepository<T>::update(const T &entity) {
    try {
        SQLite::Statement query(*_db, _updateQuery);
        _bindEntity(query, entity);
        query.bind(query.getBindParameterCount(), entity.getId());

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error updating entity in {}: {}", _tableName, e.what());
        return false;
    }
}

// remove implementation
template<typename T>
bool SQLiteRepository<T>::remove(int id) {
    try {
        SQLite::Statement query(*_db, _deleteQuery);
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error removing entity from {}: {}", _tableName, e.what());
        return false;
    }
}

#endif // SQLITE_REPOSITORY_H
