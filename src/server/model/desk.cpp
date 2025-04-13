#include "desk.h"

Desk::Desk() : _id(0), _floorNumber(0) {
}

Desk::Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber)
    : _id(id), _deskId(deskId), _buildingId(buildingId), _floorNumber(floorNumber) {
}
