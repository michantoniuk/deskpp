#ifndef COMMON_ENTITY_H
#define COMMON_ENTITY_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

// Base class for all domain entities
class Entity {
public:
    Entity() : _id(0) {
    }

    explicit Entity(int id) : _id(id) {
    }

    virtual ~Entity() = default;

    // Common functionality
    int getId() const { return _id; }
    void setId(int id) { _id = id; }

    // Polymorphic methods
    virtual json toJson() const = 0;

    virtual std::string toString() const {
        return "Entity ID: " + std::to_string(_id);
    }

protected:
    int _id; // Entity identifier
};

#endif // COMMON_ENTITY_H
