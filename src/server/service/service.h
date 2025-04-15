#ifndef SERVICE_H
#define SERVICE_H

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include "../repository/repository.h"

using json = nlohmann::json;

template<typename T>
class Service {
protected:
    Repository<T> &_repository;

    Service(Repository<T> &repository) : _repository(repository) {
    }

    // Common response generators
    json successResponse(const json &data = {});

    json errorResponse(const std::string &message);

    // Common entity list formatter
    json entityListToJson(const std::vector<T> &entities, const std::string &key);

    // Common operations that are similar across services
    json getAll(const std::string &entityKey);

    json getById(int id, const std::string &entityKey, const std::string &notFoundMsg);

    json removeById(int id, const std::string &successMsg, const std::string &notFoundMsg);
};

// Template implementations

template<typename T>
json Service<T>::successResponse(const json &data) {
    json response = {{"status", "success"}};
    if (!data.empty()) {
        response.merge_patch(data);
    }
    return response;
}

template<typename T>
json Service<T>::errorResponse(const std::string &message) {
    return {{"status", "error"}, {"message", message}};
}

template<typename T>
json Service<T>::entityListToJson(const std::vector<T> &entities, const std::string &key) {
    json array = json::array();
    for (const auto &entity: entities) {
        array.push_back(entity.toJson());
    }

    return successResponse({{key, array}});
}

template<typename T>
json Service<T>::getAll(const std::string &entityKey) {
    auto entities = _repository.findAll();
    return entityListToJson(entities, entityKey);
}

template<typename T>
json Service<T>::getById(int id, const std::string &entityKey, const std::string &notFoundMsg) {
    auto entity = _repository.findById(id);

    if (!entity) {
        return errorResponse(notFoundMsg);
    }

    return successResponse({{entityKey, entity->toJson()}});
}

template<typename T>
json Service<T>::removeById(int id, const std::string &successMsg, const std::string &notFoundMsg) {
    // First check if entity exists
    auto entity = _repository.findById(id);
    if (!entity) {
        return errorResponse(notFoundMsg);
    }

    // Delete entity
    bool success = _repository.remove(id);
    if (!success) {
        return errorResponse("Error deleting entity");
    }

    return successResponse({{"message", successMsg}});
}

#endif // SERVICE_H
