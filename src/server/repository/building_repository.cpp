#include "building_repository.h"

BuildingRepository::BuildingRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<Building>(
        db,
        "buildings",
        "SELECT id, name, address, num_floors FROM buildings ORDER BY name",
        "SELECT id, name, address, num_floors FROM buildings WHERE id = ?",
        "INSERT INTO buildings (name, address, num_floors) VALUES (?, ?, ?)",
        "UPDATE buildings SET name = ?, address = ?, num_floors = ? WHERE id = ?",
        "DELETE FROM buildings WHERE id = ?",
        buildingFromRow,
        [](SQLite::Statement &stmt, const Building &building) {
            stmt.bind(1, building.getName());
            stmt.bind(2, building.getAddress());
            stmt.bind(3, building.getNumFloors());
        }
    ) {
}

Building BuildingRepository::buildingFromRow(SQLite::Statement &query) {
    return Building(
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString(),
        query.getColumn(3).getInt()
    );
}

std::optional<Building> BuildingRepository::findByName(const std::string &name) {
    SQLite::Statement query(*_db, "SELECT id, name, address, num_floors FROM buildings WHERE name = ?");
    query.bind(1, name);

    if (query.executeStep()) {
        return buildingFromRow(query);
    }
    return std::nullopt;
}
