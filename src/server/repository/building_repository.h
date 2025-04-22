#ifndef BUILDING_REPOSITORY_H
#define BUILDING_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/building.h"
#include <memory>

/**
 * @class BuildingRepository
 * @brief Repozytorium do zarządzania budynkami w bazie danych.
 *
 * Zapewnia operacje CRUD na budynkach oraz dodatkowe funkcje
 * do wyszukiwania budynków.
 */
class BuildingRepository : public SQLiteRepository<Building> {
public:
    /**
     * @brief Konstruktor
     * @param db Współdzielony wskaźnik do bazy danych
     */
    explicit BuildingRepository(std::shared_ptr<SQLite::Database> db);

    /**
     * @brief Wyszukuje budynek po nazwie
     * @param name Nazwa budynku
     * @return Opcjonalny obiekt budynku (brak w przypadku nieznalezienia)
     */
    std::optional<Building> findByName(const std::string &name);

private:
    /**
     * @brief Konwertuje wiersz z bazy na obiekt budynku
     * @param query Zapytanie SQL z wynikami
     * @return Obiekt budynku
     */
    static Building buildingFromRow(SQLite::Statement &query);
};

#endif
