#include "desk_repository.h"
#include "../util/logger.h"

DeskRepository::DeskRepository(Database& db) : _db(db) {
}

std::vector<Desk> DeskRepository::getAllDesks() {
    std::vector<Desk> desks;

    std::string sql = "SELECT d.*, b.name as building_name "
                     "FROM desks d "
                     "JOIN buildings b ON d.building_id = b.id";

    _db.query(sql, [this, &desks](auto row) {
        desks.push_back(this->mapRowToDesk(row));
    });

    return desks;
}

std::vector<Desk> DeskRepository::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;

    std::string sql = "SELECT d.*, b.name as building_name "
                     "FROM desks d "
                     "JOIN buildings b ON d.building_id = b.id "
                     "WHERE d.building_id = " + std::to_string(buildingId);

    _db.query(sql, [this, &desks](auto row) {
        desks.push_back(this->mapRowToDesk(row));
    });

    return desks;
}

std::optional<Desk> DeskRepository::getDeskById(int id) {
    std::optional<Desk> desk;

    std::string sql = "SELECT d.*, b.name as building_name "
                     "FROM desks d "
                     "JOIN buildings b ON d.building_id = b.id "
                     "WHERE d.id = " + std::to_string(id);

    _db.query(sql, [this, &desk](auto row) {
        if (!desk) {
            desk = this->mapRowToDesk(row);
        }
    });

    return desk;
}

std::optional<Desk> DeskRepository::getDeskByCode(const std::string& deskCode) {
    std::optional<Desk> desk;

    std::string sql = "SELECT d.*, b.name as building_name "
                     "FROM desks d "
                     "JOIN buildings b ON d.building_id = b.id "
                     "WHERE d.name = '" + deskCode + "'";

    _db.query(sql, [this, &desk](auto row) {
        if (!desk) {
            desk = this->mapRowToDesk(row);
        }
    });

    return desk;
}

bool DeskRepository::isDeskAvailable(int deskId, const std::string& date) {
    bool available = true;

    std::string sql = "SELECT COUNT(*) as count FROM bookings "
                      "WHERE desk_id = " + std::to_string(deskId) +
                      " AND '" + date + "' BETWEEN date AND date_to";

    _db.query(sql, [&available](auto row) {
        int count = std::stoi(row["count"]);
        available = (count == 0);
    });

    return available;
}

Desk DeskRepository::mapRowToDesk(const std::map<std::string, std::string>& row) {
    int id = std::stoi(row.at("id"));
    std::string name = row.at("name");
    std::string buildingId = row.at("building_id");
    int floorNumber = std::stoi(row.at("floor_number"));

    return Desk(id, name, buildingId, floorNumber);
}