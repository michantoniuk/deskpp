#ifndef BUILDING_REPOSITORY_H
#define BUILDING_REPOSITORY_H

#include "common/repository.h"
#include "common/models.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>

class BuildingRepository : public Repository<Building> {
public:
    explicit BuildingRepository(std::shared_ptr<SQLite::Database> db);

    std::vector<Building> findAll() override;

    std::optional<Building> findById(int id) override;

    Building add(const Building &building) override;

    bool update(const Building &building) override;

    bool remove(int id) override;

    // Additional specialized methods
    std::optional<Building> findByName(const std::string &name);

private:
    std::shared_ptr<SQLite::Database> _db;
};

#endif // BUILDING_REPOSITORY_H
