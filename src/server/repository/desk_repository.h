#ifndef DESK_REPOSITORY_H
#define DESK_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/desk.h"
#include <memory>

class DeskRepository : public SQLiteRepository<Desk> {
public:
    explicit DeskRepository(std::shared_ptr<SQLite::Database> db);

    std::vector<Desk> findByBuildingId(int buildingId);

private:
    static Desk deskFromRow(SQLite::Statement &query);
};

#endif
