#include "building.h"

Building::Building(int id, const std::string &name, const std::string &address, int numFloors)
    : Entity(id), _name(name), _address(address), _numFloors(numFloors) {
}

json Building::toJson() const {
    json j = {
        {"id", getId()},
        {"name", _name},
        {"address", _address},
        {"numFloors", _numFloors},
        {"floors", getFloors()}
    };
    return j;
}

std::string Building::toString() const {
    return "Building: " + _name + " (ID: " + std::to_string(getId()) + ")";
}

std::vector<int> Building::getFloors() const {
    std::vector<int> floors;
    for (int i = 1; i <= _numFloors; i++) {
        floors.push_back(i);
    }
    return floors;
}
