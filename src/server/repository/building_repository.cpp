#include "building_repository.h"

#include "sqlite_repository.h"
#include "common/logger.h"

BuildingRepository::BuildingRepository(std::shared_ptr<SQLite::Database> db)
    : ::SQLiteRepository<Building>(
        db,
        "buildings",
        "SELECT id, name, address FROM buildings ORDER BY name",
        "SELECT id, name, address FROM buildings WHERE id = ?",
        "INSERT INTO buildings (name, address) VALUES (?, ?)",
        "UPDATE buildings SET name = ?, address = ? WHERE id = ?",
        "DELETE FROM buildings WHERE id = ?",
        buildingFromRow,
        [](SQLite::Statement &stmt, const Building &building) {
            stmt.bind(1, building.getName());
            stmt.bind(2, building.getAddress());
        }
    ) {
}

Building BuildingRepository::buildingFromRow(SQLite::Statement &query) {
    return Building(
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString()
    );
}

std::optional<Building> BuildingRepository::findByName(const std::string &name) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, address FROM buildings WHERE name = ?");
        query.bind(1, name);

        if (query.executeStep()) {
            return buildingFromRow(query);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting building by name: {}", e.what());
    }

    return std::nullopt;
}
