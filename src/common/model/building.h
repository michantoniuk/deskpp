#ifndef BUILDING_H
#define BUILDING_H

#include "entity.h"

class Building : public Entity {
public:
    Building() = default;

    Building(int id, const std::string &name, const std::string &address);

    json toJson() const override;

    std::string toString() const override;

    const std::string &getName() const { return _name; }
    const std::string &getAddress() const { return _address; }

    void setName(const std::string &name) { _name = name; }
    void setAddress(const std::string &address) { _address = address; }

private:
    std::string _name, _address;
};
#endif
