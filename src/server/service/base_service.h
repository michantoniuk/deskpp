#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class BaseService {
protected:
    // Common response generators
    json successResponse(const json &data = {}) {
        json response = {{"status", "success"}};
        response.merge_patch(data);
        return response;
    }

    json errorResponse(const std::string &message) {
        return {{"status", "error"}, {"message", message}};
    }

    // Common entity list formatter
    template<typename T>
    json entityListToJson(const std::vector<T> &entities, const std::string &key) {
        json array = json::array();
        for (const auto &entity: entities) {
            array.push_back(entity.toJson());
        }

        return successResponse({{key, array}});
    }
};

#endif // BASE_SERVICE_H
