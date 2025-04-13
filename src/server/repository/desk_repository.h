#ifndef DESK_REPOSITORY_H
#define DESK_REPOSITORY_H

#include <vector>
#include <string>
#include <optional>
#include "../db/database.h"
#include "../model/desk.h"

class DeskRepository {
public:
    explicit DeskRepository(Database &db);

    std::vector<Desk> getAllDesks();

    std::vector<Desk> getDesksByBuilding(int buildingId);

    std::optional<Desk> getDeskById(int id);

    std::optional<Desk> getDeskByCode(const std::string &deskCode);

    bool isDeskAvailable(int deskId, const std::string &date);

private:
    Database &_db;

    Desk mapRowToDesk(const std::map<std::string, std::string> &row);
};

#endif // DESK_REPOSITORY_H
