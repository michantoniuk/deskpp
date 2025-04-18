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

    // Helper methods
    json successResponse(const json &data = {}) {
        json response = {{"status", "success"}};
        if (!data.empty()) {
            response.merge_patch(data);
        }
        return response;
    }

    json errorResponse(const std::string &message) {
        return {{"status", "error"}, {"message", message}};
    }

    json entityListToJson(const std::vector<T> &entities, const std::string &key) {
        json array = json::array();
        for (const auto &entity: entities) {
            array.push_back(entity.toJson());
        }
        return successResponse({{key, array}});
    }
};

#endif // SERVICE_H
