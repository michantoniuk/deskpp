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
        // Get building ID from query param if provided
        auto buildingIdParam = req.url_params.get("buildingId");
        auto floorParam = req.url_params.get("floor");

        if (buildingIdParam && floorParam) {
            // Filtruj po budynku i piętrze
            int buildingId = std::stoi(buildingIdParam);
            int floor = std::stoi(floorParam);
            json result = _bookingService.getDesksByBuildingAndFloor(buildingId, floor);
            return successResponse(result);
        } else if (buildingIdParam) {
            // Filtruj tylko po budynku
            int buildingId = std::stoi(buildingIdParam);
            json result = _bookingService.getDesksByBuilding(buildingId);
            return successResponse(result);
        } else {
            // Zwróć wszystkie biurka
            json result = _bookingService.getAllDesks();
            return successResponse(result);
        }
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

        if (!dateFromParam || !dateToParam) {
            return errorResponse(400, "Missing date parameters");
        }

        json result = _bookingService.getBookingsForDesk(deskId, dateFromParam, dateToParam);
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response BookingController::addBooking(const crow::request &req) {
    try {
        LOG_INFO("Received booking request: {}", req.body);
        auto params = validateRequest(req, {"deskId", "userId", "dateFrom", "dateTo"});
        if (!params) {
            LOG_ERROR("Missing required fields in booking request");
            return errorResponse(400, "Missing required fields");
        }

        int deskId = (*params)["deskId"].get<int>();
        int userId = (*params)["userId"].get<int>();
        std::string dateFrom = (*params)["dateFrom"].get<std::string>();
        std::string dateTo = (*params)["dateTo"].get<std::string>();

        LOG_INFO("Processing booking: desk={}, user={}, from={}, to={}",
                 deskId, userId, dateFrom, dateTo);

        json result = _bookingService.addBooking(deskId, userId, dateFrom, dateTo);
        if (result.contains("status") && result["status"] == "error") {
            LOG_ERROR("Booking error: {}", result["message"].get<std::string>());
            return errorResponse(400, result["message"]);
        }

        LOG_INFO("Booking successful");
        return successResponse(result);
    } catch (const std::exception &ex) {
        LOG_ERROR("Server error during booking: {}", ex.what());
        return errorResponse(500, "Server error: " + std::string(ex.what()));
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

crow::response BookingController::getFloorsByBuilding(int buildingId) {
    try {
        json result = _bookingService.getFloorsByBuilding(buildingId);
        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(404, result["message"]);
        }
        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}
