#include "building_repository.h"
#include "../util/logger.h"

BuildingRepository::BuildingRepository(std::shared_ptr<SQLite::Database> db)
    : _db(db) {
}

std::vector<Building> BuildingRepository::findAll() {
    std::vector<Building> buildings;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, address FROM buildings ORDER BY name");

        while (query.executeStep()) {
            Building building(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString()
            );
            buildings.push_back(building);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting buildings: {}", e.what());
    }

    return buildings;
}

std::optional<Building> BuildingRepository::findById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, address FROM buildings WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Building(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting building by ID: {}", e.what());
    }

    return std::nullopt;
}

Building BuildingRepository::add(const Building &building) {
    try {
        SQLite::Statement query(*_db, "INSERT INTO buildings (name, address) VALUES (?, ?)");
        query.bind(1, building.getName());
        query.bind(2, building.getAddress());

        query.exec();
        int id = static_cast<int>(_db->getLastInsertRowid());

        Building newBuilding = building;
        newBuilding.setId(id);
        return newBuilding;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error adding building: {}", e.what());
        return building; // Return original building with ID 0 to indicate failure
    }
}

bool BuildingRepository::update(const Building &building) {
    try {
        SQLite::Statement query(*_db, "UPDATE buildings SET name = ?, address = ? WHERE id = ?");
        query.bind(1, building.getName());
        query.bind(2, building.getAddress());
        query.bind(3, building.getId());

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error updating building: {}", e.what());
        return false;
    }
}

bool BuildingRepository::remove(int id) {
    try {
        SQLite::Statement query(*_db, "DELETE FROM buildings WHERE id = ?");
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error removing building: {}", e.what());
        return false;
    }
}

std::optional<Building> BuildingRepository::findByName(const std::string &name) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, address FROM buildings WHERE name = ?");
        query.bind(1, name);

        if (query.executeStep()) {
            return Building(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting building by name: {}", e.what());
    }

    return std::nullopt;
}
