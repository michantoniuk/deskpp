#ifndef SQLITE_REPOSITORY_H
#define SQLITE_REPOSITORY_H

#include "repository.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include "common/logger.h"

/**
 * @class SQLiteRepository
 * @brief Bazowa implementacja repozytorium dla bazy SQLite.
 *
 * Zapewnia standardowe operacje CRUD na bazie SQLite.
 *
 * @tparam T Typ encji obsługiwanej przez repozytorium
 */
template<typename T>
class SQLiteRepository : public Repository<T> {
protected:
    std::shared_ptr<SQLite::Database> _db;
    std::string _tableName;
    std::string _findAllQuery;
    std::string _findByIdQuery;
    std::string _insertQuery;
    std::string _updateQuery;
    std::string _deleteQuery;
    std::function<T(SQLite::Statement &)> _rowToEntity;
    std::function<void(SQLite::Statement &, const T &)> _bindEntity;

public:
    /**
     * @brief Konstruktor
     * @param db Współdzielony wskaźnik do bazy danych
     * @param tableName Nazwa tabeli
     * @param findAllQuery Zapytanie SELECT dla wszystkich rekordów
     * @param findByIdQuery Zapytanie SELECT dla pojedynczego rekordu
     * @param insertQuery Zapytanie INSERT
     * @param updateQuery Zapytanie UPDATE
     * @param deleteQuery Zapytanie DELETE
     * @param rowToEntity Funkcja konwertująca wiersz na encję
     * @param bindEntity Funkcja wiążąca encję z parametrami zapytania
     */
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
    ) : _db(db), _tableName(tableName), _findAllQuery(findAllQuery),
        _findByIdQuery(findByIdQuery), _insertQuery(insertQuery),
        _updateQuery(updateQuery), _deleteQuery(deleteQuery),
        _rowToEntity(rowToEntity), _bindEntity(bindEntity) {
    }

    /**
     * @brief Pobiera wszystkie encje
     * @return Wektor wszystkich encji
     */
    std::vector<T> findAll() override {
        std::vector<T> entities;
        SQLite::Statement query(*_db, _findAllQuery);

        while (query.executeStep()) {
            entities.push_back(_rowToEntity(query));
        }
        return entities;
    }

    /**
     * @brief Pobiera encję po identyfikatorze
     * @param id Identyfikator encji
     * @return Opcjonalny obiekt encji (brak w przypadku nieznalezienia)
     */
    std::optional<T> findById(int id) override {
        SQLite::Statement query(*_db, _findByIdQuery);
        query.bind(1, id);

        if (query.executeStep()) {
            return _rowToEntity(query);
        }
        return std::nullopt;
    }

    /**
     * @brief Dodaje nową encję
     * @param entity Obiekt encji do dodania
     * @return Dodana encja (z zaktualizowanym identyfikatorem)
     */
    T add(const T &entity) override {
        SQLite::Statement query(*_db, _insertQuery);
        _bindEntity(query, entity);

        query.exec();
        int id = static_cast<int>(_db->getLastInsertRowid());

        T newEntity = entity;
        newEntity.setId(id);
        return newEntity;
    }

    /**
     * @brief Aktualizuje istniejącą encję
     * @param entity Obiekt encji do aktualizacji
     * @return Czy operacja się powiodła
     */
    bool update(const T &entity) override {
        SQLite::Statement query(*_db, _updateQuery);
        _bindEntity(query, entity);
        query.bind(query.getBindParameterCount(), entity.getId());

        query.exec();
        return query.getChanges() > 0;
    }

    /**
     * @brief Usuwa encję
     * @param id Identyfikator encji do usunięcia
     * @return Czy operacja się powiodła
     */
    bool remove(int id) override {
        SQLite::Statement query(*_db, _deleteQuery);
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    }
};

#endif
