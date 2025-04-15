#include "desk_repository.h"
#include "../util/logger.h"

DeskRepository::DeskRepository(std::shared_ptr<SQLite::Database> db)
    : _db(db) {
}

std::vector<Desk> DeskRepository::findAll() {
    std::vector<Desk> desks;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number FROM desks");

        while (query.executeStep()) {
            Desk desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
            desks.push_back(desk);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting all desks: {}", e.what());
    }

    return desks;
}

std::optional<Desk> DeskRepository::findById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desk by ID: {}", e.what());
    }

    return std::nullopt;
}

Desk DeskRepository::add(const Desk &desk) {
    try {
        SQLite::Statement query(*_db, "INSERT INTO desks (name, building_id, floor_number) "
                                "VALUES (?, ?, ?)");
        query.bind(1, desk.getDeskId());
        query.bind(2, std::stoi(desk.getBuildingId()));
        query.bind(3, desk.getFloorNumber());

        query.exec();
        int id = static_cast<int>(_db->getLastInsertRowid());

        Desk newDesk = desk;
        newDesk.setId(id);
        return newDesk;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error adding desk: {}", e.what());
        return desk; // Return original desk with ID 0 to indicate failure
    } catch (const std::invalid_argument &e) {
        LOG_ERROR("Error converting building ID to integer: {}", e.what());
        return desk;
    }
}

bool DeskRepository::update(const Desk &desk) {
    try {
        SQLite::Statement query(*_db, "UPDATE desks SET name = ?, building_id = ?, floor_number = ? "
                                "WHERE id = ?");
        query.bind(1, desk.getDeskId());
        query.bind(2, std::stoi(desk.getBuildingId()));
        query.bind(3, desk.getFloorNumber());
        query.bind(4, desk.getId());

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error updating desk: {}", e.what());
        return false;
    } catch (const std::invalid_argument &e) {
        LOG_ERROR("Error converting building ID to integer: {}", e.what());
        return false;
    }
}

bool DeskRepository::remove(int id) {
    try {
        SQLite::Statement query(*_db, "DELETE FROM desks WHERE id = ?");
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error removing desk: {}", e.what());
        return false;
    }
}

std::vector<Desk> DeskRepository::findByBuildingId(int buildingId) {
    std::vector<Desk> desks;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE building_id = ?");
        query.bind(1, buildingId);

        while (query.executeStep()) {
            Desk desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
            desks.push_back(desk);
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
            Desk desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
            desks.push_back(desk);
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
            return Desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desk by desk number: {}", e.what());
    }

    return std::nullopt;
}
