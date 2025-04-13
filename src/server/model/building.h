#ifndef SERVER_MODEL_BUILDING_H
#define SERVER_MODEL_BUILDING_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Building {
public:
    Building();
    Building(int id, const std::string &name, const std::string &address);

    // Getters
    int getId() const { return _id; }
    const std::string& getName() const { return _name; }
    const std::string& getAddress() const { return _address; }

    // Setters
    void setId(int id) { _id = id; }
    void setName(const std::string &name) { _name = name; }
    void setAddress(const std::string &address) { _address = address; }

    // Convert to JSON
    json toJson() const {
        json j;
        j["id"] = _id;
        j["name"] = _name;
        j["address"] = _address;
        return j;
    }

private:
    int _id;
    std::string _name;
    std::string _address;
};

#endif // SERVER_MODEL_BUILDING_H