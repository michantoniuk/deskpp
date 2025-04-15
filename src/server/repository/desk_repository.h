#ifndef DESK_REPOSITORY_H
#define DESK_REPOSITORY_H

#include "common/repository.h"
#include "common/models.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>

class DeskRepository : public Repository<Desk> {
public:
    explicit DeskRepository(std::shared_ptr<SQLite::Database> db);

    std::vector<Desk> findAll() override;

    std::optional<Desk> findById(int id) override;

    Desk add(const Desk &desk) override;

    bool update(const Desk &desk) override;

    bool remove(int id) override;

    // Additional specialized methods
    std::vector<Desk> findByBuildingId(int buildingId);

    std::vector<Desk> findByFloorNumber(int buildingId, int floorNumber);

    std::optional<Desk> findByDeskNumber(const std::string &deskId);

private:
    std::shared_ptr<SQLite::Database> _db;
};

#endif // DESK_REPOSITORY_H
