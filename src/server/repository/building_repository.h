#ifndef BUILDING_REPOSITORY_H
#define BUILDING_REPOSITORY_H

#include "repository.h"
#include "../../common/model/model.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>

#include "sqlite_repository.h"

class BuildingRepository : public SQLiteRepository<Building> {
public:
    explicit BuildingRepository(std::shared_ptr<SQLite::Database> db);

    // Additional specialized methods
    std::optional<Building> findByName(const std::string &name);

private:
    // Helper for loading building from query result
    static Building buildingFromRow(SQLite::Statement &query);
};

#endif // BUILDING_REPOSITORY_H
