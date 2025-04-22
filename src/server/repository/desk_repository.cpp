#include "desk_repository.h"

DeskRepository::DeskRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<Desk>(
        db,
        "desks",
        "SELECT id, name, building_id, floor FROM desks",
        "SELECT id, name, building_id, floor FROM desks WHERE id = ?",
        "INSERT INTO desks (name, building_id, floor) VALUES (?, ?, ?)",
        "UPDATE desks SET name = ?, building_id = ?, floor = ? WHERE id = ?",
        "DELETE FROM desks WHERE id = ?",
        deskFromRow,
        [](SQLite::Statement &stmt, const Desk &desk) {
            stmt.bind(1, desk.getName());
            stmt.bind(2, desk.getBuildingId());
            stmt.bind(3, desk.getFloor());
        }
    ) {
}

Desk DeskRepository::deskFromRow(SQLite::Statement &query) {
    int id = query.getColumn(0).getInt();
    std::string name = query.getColumn(1).getString();
    int buildingId = query.getColumn(2).getInt();
    int floor = query.getColumn(3).getInt();
    return Desk(id, name, buildingId, floor);
}

std::vector<Desk> DeskRepository::findByBuildingId(int buildingId) {
    std::vector<Desk> desks;
    SQLite::Statement query(*_db, "SELECT id, name, building_id, floor FROM desks WHERE building_id = ?");
    query.bind(1, buildingId);

    while (query.executeStep()) {
        desks.push_back(deskFromRow(query));
    }
    return desks;
}
