#include "building.h"

Building::Building() : _id(0) {
}

Building::Building(int id, const std::string &name, const std::string &address)
    : _id(id), _name(name), _address(address) {
}