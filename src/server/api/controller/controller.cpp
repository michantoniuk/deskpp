#include "controller.h"

json Controller::parseJson(const std::string &body) {
    try {
        return json::parse(body);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error parsing JSON: {}", ex.what());
        throw std::runtime_error("Invalid JSON format");
    }
}

crow::response Controller::errorResponse(int statusCode, const std::string &message) {
    json response = {
        {"status", "error"},
        {"message", message}
    };
    return crow::response(statusCode, response.dump());
}

crow::response Controller::successResponse(const json &data) {
    return crow::response(200, data.dump());
}

std::optional<json> Controller::validateRequest(const crow::request &req,
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
