#ifndef BUILDING_H
#define BUILDING_H

#include "entity.h"
#include <vector>

class Building : public Entity {
public:
    Building() = default;

    Building(int id, const std::string &name, const std::string &address, int numFloors = 1);

    json toJson() const override;

    std::string toString() const override;

    const std::string &getName() const { return _name; }
    const std::string &getAddress() const { return _address; }
    int getNumFloors() const { return _numFloors; }

    std::vector<int> getFloors() const;

    void setName(const std::string &name) { _name = name; }
    void setAddress(const std::string &address) { _address = address; }
    void setNumFloors(int numFloors) { _numFloors = numFloors; }

private:
    std::string _name;
    std::string _address;
    int _numFloors = 1;
};
#endif
