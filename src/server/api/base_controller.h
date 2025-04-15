#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include "../util/logger.h"

using json = nlohmann::json;

class BaseController {
protected:
    json parseJson(const std::string &body) {
        try {
            return json::parse(body);
        } catch (const std::exception &ex) {
            LOG_ERROR("Error parsing JSON: {}", ex.what());
            throw std::runtime_error("Invalid JSON format");
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

    std::optional<json> validateRequest(const crow::request &req,
                                        const std::vector<std::string> &requiredFields) {
        try {
            json params = parseJson(req.body);

            // Check required fields
            for (const auto &field: requiredFields) {
                if (!params.contains(field) || params[field].is_null()) {
                    return std::nullopt;
                }
            }

            return params;
        } catch (...) {
            return std::nullopt;
        }
    }

    // Helper method for standardized try-catch blocks
    template<typename Func>
    crow::response tryCatchResponse(Func &&func) {
        try {
            return func();
        } catch (const std::exception &ex) {
            LOG_ERROR("Error: {}", ex.what());
            return errorResponse(500, "Server error");
        }
    }
};

#endif // BASE_CONTROLLER_H
