#include "desk_repository.h"

DeskRepository::DeskRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<Desk>(
        db,
        "desks",
        "SELECT id, name, building_id, floor_number, location_x, location_y FROM desks",
        "SELECT id, name, building_id, floor_number, location_x, location_y FROM desks WHERE id = ?",
        "INSERT INTO desks (name, building_id, floor_number, location_x, location_y) VALUES (?, ?, ?, ?, ?)",
        "UPDATE desks SET name = ?, building_id = ?, floor_number = ?, location_x = ?, location_y = ? WHERE id = ?",
        "DELETE FROM desks WHERE id = ?",
        deskFromRow,
        [](SQLite::Statement &stmt, const Desk &desk) {
            stmt.bind(1, desk.getDeskId());
            stmt.bind(2, std::stoi(desk.getBuildingId()));
            stmt.bind(3, desk.getFloorNumber());
            stmt.bind(4, desk.getLocationX());
            stmt.bind(5, desk.getLocationY());
        }
    ) {
}

Desk DeskRepository::deskFromRow(SQLite::Statement &query) {
    int id = query.getColumn(0).getInt();
    std::string deskId = query.getColumn(1).getString();
    std::string buildingId = std::to_string(query.getColumn(2).getInt());
    int floorNumber = query.getColumn(3).getInt();
    int locationX = query.getColumnCount() > 4 ? query.getColumn(4).getInt() : 0;
    int locationY = query.getColumnCount() > 5 ? query.getColumn(5).getInt() : 0;

    return Desk(id, deskId, buildingId, floorNumber, locationX, locationY);
}

std::vector<Desk> DeskRepository::findByBuildingId(int buildingId) {
    std::vector<Desk> desks;

    SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number, location_x, location_y "
                            "FROM desks WHERE building_id = ?");
    query.bind(1, buildingId);

    while (query.executeStep()) {
        desks.push_back(deskFromRow(query));
    }

    return desks;
}

std::vector<Desk> DeskRepository::findByFloorNumber(int buildingId, int floorNumber) {
    std::vector<Desk> desks;

    SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number, location_x, location_y "
                            "FROM desks WHERE building_id = ? AND floor_number = ?");
    query.bind(1, buildingId);
    query.bind(2, floorNumber);

    while (query.executeStep()) {
        desks.push_back(deskFromRow(query));
    }

    return desks;
}

std::optional<Desk> DeskRepository::findByDeskNumber(const std::string &deskId) {
    SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number, location_x, location_y "
                            "FROM desks WHERE name = ?");
    query.bind(1, deskId);

    if (query.executeStep()) {
        return deskFromRow(query);
    }

    return std::nullopt;
}
