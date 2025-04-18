#include "admin_controller.h"

AdminController::AdminController(BookingService &bookingService)
    : _bookingService(bookingService) {
}

crow::response AdminController::addBuilding(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"name", "address"});
        if (!params) {
            return errorResponse(400, "Missing required fields");
        }

        std::string name = (*params)["name"].get<std::string>();
        std::string address = (*params)["address"].get<std::string>();

        json result = _bookingService.addBuilding(name, address);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response AdminController::updateBuilding(int id, const crow::request &req) {
    try {
        auto params = validateRequest(req, {"name", "address"});
        if (!params) {
            return errorResponse(400, "Missing required fields");
        }

        std::string name = (*params)["name"].get<std::string>();
        std::string address = (*params)["address"].get<std::string>();

        json result = _bookingService.updateBuilding(id, name, address);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response AdminController::deleteBuilding(int id) {
    try {
        json result = _bookingService.deleteBuilding(id);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response AdminController::addDesk(const crow::request &req) {
    try {
        auto params = validateRequest(req, {"deskId", "buildingId", "floorNumber"});
        if (!params) {
            return errorResponse(400, "Missing required fields");
        }

        std::string deskId = (*params)["deskId"].get<std::string>();
        int buildingId = (*params)["buildingId"].get<int>();
        int floorNumber = (*params)["floorNumber"].get<int>();

        // Optional fields
        int locationX = 0;
        int locationY = 0;

        if (params->contains("locationX")) {
            locationX = (*params)["locationX"].get<int>();
        }

        if (params->contains("locationY")) {
            locationY = (*params)["locationY"].get<int>();
        }

        json result = _bookingService.addDesk(deskId, buildingId, floorNumber, locationX, locationY);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response AdminController::updateDesk(int id, const crow::request &req) {
    try {
        json deskData = parseJson(req.body);

        json result = _bookingService.updateDesk(id, deskData);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}

crow::response AdminController::deleteDesk(int id) {
    try {
        json result = _bookingService.deleteDesk(id);

        if (result.contains("status") && result["status"] == "error") {
            return errorResponse(400, result["message"]);
        }

        return successResponse(result);
    } catch (const std::exception &ex) {
        return errorResponse(500, "Server error");
    }
}
