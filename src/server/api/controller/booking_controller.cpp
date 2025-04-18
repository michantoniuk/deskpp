#include "booking_controller.h"

BookingController::BookingController(BookingService &bookingService)
    : _bookingService(bookingService) {
}

crow::response BookingController::getBuildings(const crow::request &req) {
    try {
        json result = _bookingService.getAllBuildings();
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::getDesks(const crow::request &req) {
    try {
        int buildingId = -1;

        // Get building ID from query param if provided
        auto buildingIdParam = req.url_params.get("buildingId");
        if (buildingIdParam) {
            buildingId = std::stoi(buildingIdParam);
        }

        json result = _bookingService.getDesksByBuilding(buildingId);
        return successResponse(result);
    } catch (const std::exception &ex) {
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

        // Check for date params
        auto dateFromParam = req.url_params.get("dateFrom");
        auto dateToParam = req.url_params.get("dateTo");
        auto dateParam = req.url_params.get("date");

        json result;

        if (dateFromParam && dateToParam) {
            // Date range
            result = _bookingService.getBookingsForDeskInRange(deskId, dateFromParam, dateToParam);
        } else if (dateParam) {
            // Single date
            result = _bookingService.getBookingsForDesk(deskId, dateParam);
        } else {
            return errorResponse(400, "Missing date parameters");
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::addBooking(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"deskId", "userId", "dateFrom", "dateTo"});
        if (!params) {
            return errorResponse(400, "Missing required fields");
        }

        int deskId = (*params)["deskId"].get<int>();
        int userId = (*params)["userId"].get<int>();
        std::string dateFrom = (*params)["dateFrom"].get<std::string>();
        std::string dateTo = (*params)["dateTo"].get<std::string>();

        json result = _bookingService.addBooking(deskId, userId, dateFrom, dateTo);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::cancelBooking(int bookingId) {
    try {
        json result = _bookingService.cancelBooking(bookingId);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(404, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}
