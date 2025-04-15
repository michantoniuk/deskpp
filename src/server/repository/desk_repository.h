#ifndef DESK_REPOSITORY_H
#define DESK_REPOSITORY_H

#include "sqlite_repository.h"
#include "../../common/model/model.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>

class DeskRepository : public SQLiteRepository<Desk> {
public:
    explicit DeskRepository(std::shared_ptr<SQLite::Database> db);

    // Additional specialized methods
    std::vector<Desk> findByBuildingId(int buildingId);

    std::vector<Desk> findByFloorNumber(int buildingId, int floorNumber);

    std::optional<Desk> findByDeskNumber(const std::string &deskId);

private:
    // Helper for loading desk from query result
    static Desk deskFromRow(SQLite::Statement &query);
};

#endif // DESK_REPOSITORY_H
