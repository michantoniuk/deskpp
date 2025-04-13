#include "booking_controller.h"
#include "../util/logger.h"

BookingController::BookingController(BookingService &bookingService)
    : _bookingService(bookingService) {
}

crow::response BookingController::getBuildings(const crow::request &req) {
    try {
        json result = _bookingService.getAllBuildings();
        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error getting buildings: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::getDesks(const crow::request &req) {
    try {
        int buildingId = -1;
        auto buildingIdParam = req.url_params.get("buildingId");

        if (buildingIdParam) {
            try {
                buildingId = std::stoi(buildingIdParam);
                LOG_INFO("Getting desks for building ID: {}", buildingId);
            } catch (...) {
                LOG_ERROR("Invalid buildingId parameter");
                return errorResponse(400, "Invalid buildingId parameter");
            }
        }

        json result = _bookingService.getDesksByBuilding(buildingId);
        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error getting desks: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::getBookings(const crow::request &req) {
    try {
        auto deskIdParam = req.url_params.get("deskId");
        if (!deskIdParam) {
            return errorResponse(400, "Missing deskId parameter");
        }

        int deskId = std::stoi(deskIdParam);

        // Check for date range or single date
        auto dateFromParam = req.url_params.get("dateFrom");
        auto dateToParam = req.url_params.get("dateTo");
        auto dateParam = req.url_params.get("date");

        json result;

        if (dateFromParam && dateToParam) {
            // Date range
            std::string dateFrom = dateFromParam;
            std::string dateTo = dateToParam;
            result = _bookingService.getBookingsForDeskInRange(deskId, dateFrom, dateTo);
        } else if (dateParam) {
            // Single date
            std::string date = dateParam;
            result = _bookingService.getBookingsForDesk(deskId, date);
        } else {
            return errorResponse(400, "Missing date parameters");
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error getting bookings: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::addBooking(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"deskId", "userId", "dateFrom", "dateTo"});
        if (!params) {
            return errorResponse(400, "Missing or invalid required fields");
        }

        int deskId = (*params)["deskId"].get<int>();
        int userId = (*params)["userId"].get<int>();
        std::string dateFrom = (*params)["dateFrom"].get<std::string>();
        std::string dateTo = (*params)["dateTo"].get<std::string>();

        LOG_INFO("Adding booking: desk={}, user={}, from={}, to={}",
                 deskId, userId, dateFrom, dateTo);

        json result = _bookingService.addBooking(deskId, userId, dateFrom, dateTo);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error adding booking";
            return errorResponse(400, message);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error adding booking: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::cancelBooking(int bookingId) {
    try {
        LOG_INFO("Cancelling booking ID: {}", bookingId);

        json result = _bookingService.cancelBooking(bookingId);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error cancelling booking";
            return errorResponse(404, message);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error cancelling booking: {}", ex.what());
        return errorResponse(500, "Server error");
    }
}

json BookingController::parseJson(const std::string &body) {
    try {
        return json::parse(body);
    } catch (const std::exception &ex) {
        LOG_ERROR("Error parsing JSON: {}", ex.what());
        throw std::runtime_error("Invalid JSON format");
    }
}

crow::response BookingController::errorResponse(int statusCode, const std::string &message) {
    json response = {
        {"status", "error"},
        {"message", message}
    };

    return crow::response(statusCode, response.dump());
}

crow::response BookingController::successResponse(const json &data) {
    return crow::response(200, data.dump());
}

std::optional<json> BookingController::validateRequest(const crow::request &req,
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
