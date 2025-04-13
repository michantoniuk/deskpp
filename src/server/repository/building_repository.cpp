#include "building_repository.h"
#include "../util/logger.h"

BuildingRepository::BuildingRepository(Database& db) : _db(db) {
}

std::vector<Building> BuildingRepository::getAllBuildings() {
    std::vector<Building> buildings;

    std::string sql = "SELECT id, name, address FROM buildings ORDER BY name";

    _db.query(sql, [this, &buildings](auto row) {
        buildings.push_back(this->mapRowToBuilding(row));
    });

    return buildings;
}

std::optional<Building> BuildingRepository::getBuildingById(int id) {
    std::optional<Building> building;

    std::string sql = "SELECT id, name, address FROM buildings WHERE id = " + std::to_string(id);

    _db.query(sql, [this, &building](auto row) {
        if (!building) {
            building = this->mapRowToBuilding(row);
        }
    });

    return building;
}

std::optional<Building> BuildingRepository::getBuildingByName(const std::string& name) {
    std::optional<Building> building;

    // Using SQL LIKE to ensure case-insensitive matching
    std::string sql = "SELECT id, name, address FROM buildings WHERE name LIKE '" + name + "'";

    _db.query(sql, [this, &building](auto row) {
        if (!building) {
            building = this->mapRowToBuilding(row);
        }
    });

    return building;
}

Building BuildingRepository::mapRowToBuilding(const std::map<std::string, std::string>& row) {
    int id = std::stoi(row.at("id"));
    std::string name = row.at("name");
    std::string address = row.at("address");

    return Building(id, name, address);
}