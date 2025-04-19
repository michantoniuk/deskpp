#ifndef ENTITY_H
#define ENTITY_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class Entity {
public:
    Entity() : _id(0) {
    }

    explicit Entity(int id) : _id(id) {
    }

    virtual ~Entity() = default;

    int getId() const { return _id; }
    void setId(int id) { _id = id; }

    virtual json toJson() const = 0;

    virtual std::string toString() const { return "Entity ID: " + std::to_string(_id); }

protected:
    int _id;
};

#endif // ENTITY_H
