#include "building.h"

Building::Building(int id, const std::string &name, const std::string &address)
    : Entity(id), _name(name), _address(address) {
}

json Building::toJson() const {
    return {
        {"id", getId()},
        {"name", _name},
        {"address", _address}
    };
}

std::string Building::toString() const {
    return "Building: " + _name + " (ID: " + std::to_string(getId()) + ")";
}
