#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include "common/logger.h"

using json = nlohmann::json;

class Controller {
protected:
    // Helper methods
    json parseJson(const std::string &body) {
        try {
            return json::parse(body);
        } catch (...) {
            return json::object();
        }
    }

    crow::response errorResponse(int statusCode, const std::string &message) {
        json response = {
            {"status", "error"},
            {"message", message}
        };
        return crow::response(statusCode, response.dump());
    }

    crow::response successResponse(const json &data) {
        return crow::response(200, data.dump());
    }

    std::optional<json> validateRequest(const crow::request &req, const std::vector<std::string> &requiredFields) {
        json params = parseJson(req.body);
        // Check required fields
        for (const auto &field: requiredFields) {
            if (!params.contains(field)) {
                return std::nullopt;
            }
        }
        return params;
    }
};

#endif
