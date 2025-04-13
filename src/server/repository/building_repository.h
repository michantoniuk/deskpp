#ifndef BUILDING_REPOSITORY_H
#define BUILDING_REPOSITORY_H

#include <vector>
#include <string>
#include <optional>
#include "../db/database.h"
#include "../model/building.h"

class BuildingRepository {
public:
    explicit BuildingRepository(Database &db);

    // Query methods
    std::vector<Building> getAllBuildings();

    std::optional<Building> getBuildingById(int id);

    std::optional<Building> getBuildingByName(const std::string &name);

private:
    Database &_db;

    // Helper methods
    Building mapRowToBuilding(const std::map<std::string, std::string> &row);
};

#endif // BUILDING_REPOSITORY_H
