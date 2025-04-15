#include "desk_repository.h"
#include "common/logger.h"

DeskRepository::DeskRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<Desk>(
        db,
        "desks",
        "SELECT id, name, building_id, floor_number FROM desks",
        "SELECT id, name, building_id, floor_number FROM desks WHERE id = ?",
        "INSERT INTO desks (name, building_id, floor_number) VALUES (?, ?, ?)",
        "UPDATE desks SET name = ?, building_id = ?, floor_number = ? WHERE id = ?",
        "DELETE FROM desks WHERE id = ?",
        deskFromRow,
        [](SQLite::Statement &stmt, const Desk &desk) {
            stmt.bind(1, desk.getDeskId());
            try {
                stmt.bind(2, std::stoi(desk.getBuildingId()));
            } catch (const std::invalid_argument &) {
                LOG_ERROR("Invalid building ID format: {}", desk.getBuildingId());
                stmt.bind(2, 0); // Default value in case of conversion error
            }
            stmt.bind(3, desk.getFloorNumber());
        }
    ) {
}

Desk DeskRepository::deskFromRow(SQLite::Statement &query) {
    return Desk(
        query.getColumn(0).getInt(),
        query.getColumn(1).getString(),
        query.getColumn(2).getInt(),
        query.getColumn(3).getInt()
    );
}

std::vector<Desk> DeskRepository::findByBuildingId(int buildingId) {
    std::vector<Desk> desks;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE building_id = ?");
        query.bind(1, buildingId);

        while (query.executeStep()) {
            desks.push_back(deskFromRow(query));
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desks by building: {}", e.what());
    }

    return desks;
}

std::vector<Desk> DeskRepository::findByFloorNumber(int buildingId, int floorNumber) {
    std::vector<Desk> desks;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE building_id = ? AND floor_number = ?");
        query.bind(1, buildingId);
        query.bind(2, floorNumber);

        while (query.executeStep()) {
            desks.push_back(deskFromRow(query));
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desks by floor: {}", e.what());
    }

    return desks;
}

std::optional<Desk> DeskRepository::findByDeskNumber(const std::string &deskId) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE name = ?");
        query.bind(1, deskId);

        if (query.executeStep()) {
            return deskFromRow(query);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desk by desk number: {}", e.what());
    }

    return std::nullopt;
}
