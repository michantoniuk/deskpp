#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include <functional>
#include "common/logger.h"

using json = nlohmann::json;

class Controller {
protected:
    // Parse JSON from request
    json parseJson(const std::string &body);

    // Common response generators
    crow::response errorResponse(int statusCode, const std::string &message);

    crow::response successResponse(const json &data);

    // Validate request fields
    std::optional<json> validateRequest(const crow::request &req,
                                        const std::vector<std::string> &requiredFields);

    // Helper method for standardized try-catch blocks
    template<typename Func>
    crow::response tryCatchResponse(Func &&func);

    // Common method for GET requests
    template<typename ServiceType>
    crow::response handleGetAll(ServiceType &service, std::function<json(ServiceType &)> serviceMethod);

    // Common method for GET by ID requests
    template<typename ServiceType>
    crow::response handleGetById(ServiceType &service, int id, std::function<json(ServiceType &, int)> serviceMethod);

    // Common method for DELETE requests
    template<typename ServiceType>
    crow::response handleDelete(ServiceType &service, int id, std::function<json(ServiceType &, int)> serviceMethod);
};

// Template implementations
template<typename Func>
crow::response Controller::tryCatchResponse(Func &&func) {
    try {
        return func();
    } catch (const std::exception &ex) {
        LOG_ERROR("Error: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

template<typename ServiceType>
crow::response Controller::handleGetAll(ServiceType &service, std::function<json(ServiceType &)> serviceMethod) {
    return tryCatchResponse([&]() {
        json result = serviceMethod(service);
        return successResponse(result);
    });
}

template<typename ServiceType>
crow::response Controller::handleGetById(ServiceType &service, int id,
                                         std::function<json(ServiceType &, int)> serviceMethod) {
    return tryCatchResponse([&]() {
        json result = serviceMethod(service, id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Entity not found";
            return errorResponse(404, message);
        }

        return successResponse(result);
    });
}

template<typename ServiceType>
crow::response Controller::handleDelete(ServiceType &service, int id,
                                        std::function<json(ServiceType &, int)> serviceMethod) {
    return tryCatchResponse([&]() {
        json result = serviceMethod(service, id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error processing request";
            return errorResponse(404, message);
        }

        return successResponse(result);
    });
}

#endif // CONTROLLER_H
