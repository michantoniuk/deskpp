#ifndef DESK_REPOSITORY_H
#define DESK_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/desk.h"
#include <memory>

/**
 * @class DeskRepository
 * @brief Repozytorium do zarządzania biurkami w bazie danych.
 *
 * Zapewnia operacje CRUD na biurkach oraz dodatkowe funkcje
 * do wyszukiwania biurek.
 */
class DeskRepository : public SQLiteRepository<Desk> {
public:
    /**
     * @brief Konstruktor
     * @param db Współdzielony wskaźnik do bazy danych
     */
    explicit DeskRepository(std::shared_ptr<SQLite::Database> db);

    /**
     * @brief Wyszukuje biurka dla wybranego budynku
     * @param buildingId Identyfikator budynku
     * @return Wektor biurek
     */
    std::vector<Desk> findByBuildingId(int buildingId);

private:
    /**
     * @brief Konwertuje wiersz z bazy na obiekt biurka
     * @param query Zapytanie SQL z wynikami
     * @return Obiekt biurka
     */
    static Desk deskFromRow(SQLite::Statement &query);
};

#endif
