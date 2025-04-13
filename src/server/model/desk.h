#ifndef SERVER_MODEL_DESK_H
#define SERVER_MODEL_DESK_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Desk {
public:
    Desk();

    Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber);

    // Getters
    int getId() const { return _id; }
    const std::string &getDeskId() const { return _deskId; }
    const std::string &getBuildingId() const { return _buildingId; }
    int getFloorNumber() const { return _floorNumber; }

    // Setters
    void setId(int id) { _id = id; }
    void setDeskId(const std::string &deskId) { _deskId = deskId; }
    void setBuildingId(const std::string &buildingId) { _buildingId = buildingId; }
    void setFloorNumber(int floorNumber) { _floorNumber = floorNumber; }

    // Convert to JSON
    json toJson() const {
        json j;
        j["id"] = _id;
        j["deskId"] = _deskId;
        j["buildingId"] = _buildingId;
        j["floorNumber"] = _floorNumber;
        return j;
    }

private:
    int _id;
    std::string _deskId;
    std::string _buildingId;
    int _floorNumber;
};

#endif // SERVER_MODEL_DESK_H
