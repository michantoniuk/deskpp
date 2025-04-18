#ifndef BUILDING_REPOSITORY_H
#define BUILDING_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/building.h"
#include <memory>

class BuildingRepository : public SQLiteRepository<Building> {
public:
    explicit BuildingRepository(std::shared_ptr<SQLite::Database> db);

    std::optional<Building> findByName(const std::string &name);

private:
    static Building buildingFromRow(SQLite::Statement &query);
};

#endif // BUILDING_REPOSITORY_H
