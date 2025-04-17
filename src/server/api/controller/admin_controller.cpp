// src/server/api/controller/admin_controller.cpp

#include "admin_controller.h"
#include "common/logger.h"

AdminController::AdminController(BookingService &bookingService)
    : _bookingService(bookingService) {
}

crow::response AdminController::addBuilding(const crow::request &req) {
    return tryCatchResponse([&]() {
        auto params = validateRequest(req, {"name", "address"});
        if (!params) {
            return errorResponse(400, "Missing or invalid required fields");
        }

        std::string name = (*params)["name"].get<std::string>();
        std::string address = (*params)["address"].get<std::string>();

        LOG_INFO("Adding building: name={}, address={}", name, address);

        json result = _bookingService.addBuilding(name, address);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error adding building";
            return errorResponse(400, message);
        }

        return successResponse(result);
    });
}

crow::response AdminController::updateBuilding(int id, const crow::request &req) {
    return tryCatchResponse([&]() {
        auto params = validateRequest(req, {"name", "address"});
        if (!params) {
            return errorResponse(400, "Missing or invalid required fields");
        }

        std::string name = (*params)["name"].get<std::string>();
        std::string address = (*params)["address"].get<std::string>();

        LOG_INFO("Updating building: id={}, name={}, address={}", id, name, address);

        json result = _bookingService.updateBuilding(id, name, address);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error updating building";
            return errorResponse(400, message);
        }

        return successResponse(result);
    });
}

crow::response AdminController::deleteBuilding(int id) {
    return tryCatchResponse([&]() {
        LOG_INFO("Deleting building: id={}", id);

        json result = _bookingService.deleteBuilding(id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error deleting building";
            return errorResponse(400, message);
        }

        return successResponse(result);
    });
}

crow::response AdminController::addDesk(const crow::request &req) {
    return tryCatchResponse([&]() {
        auto params = validateRequest(req, {"deskId", "buildingId", "floorNumber"});
        if (!params) {
            return errorResponse(400, "Missing or invalid required fields");
        }

        std::string deskId = (*params)["deskId"].get<std::string>();
        int buildingId = (*params)["buildingId"].get<int>();
        int floorNumber = (*params)["floorNumber"].get<int>();

        // Optional location coordinates
        int locationX = 0;
        int locationY = 0;

        if (params->contains("locationX") && !(*params)["locationX"].is_null()) {
            locationX = (*params)["locationX"].get<int>();
        }

        if (params->contains("locationY") && !(*params)["locationY"].is_null()) {
            locationY = (*params)["locationY"].get<int>();
        }

        LOG_INFO("Adding desk: deskId={}, buildingId={}, floor={}, position=({},{})",
                 deskId, buildingId, floorNumber, locationX, locationY);

        json result = _bookingService.addDesk(deskId, buildingId, floorNumber, locationX, locationY);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error adding desk";
            return errorResponse(400, message);
        }

        return successResponse(result);
    });
}

crow::response AdminController::updateDesk(int id, const crow::request &req) {
    return tryCatchResponse([&]() {
        auto params = parseJson(req.body);
        if (params.empty()) {
            return errorResponse(400, "Invalid request format");
        }

        LOG_INFO("Updating desk: id={}", id);

        json result = _bookingService.updateDesk(id, params);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error updating desk";
            return errorResponse(400, message);
        }

        return successResponse(result);
    });
}

crow::response AdminController::deleteDesk(int id) {
    return tryCatchResponse([&]() {
        LOG_INFO("Deleting desk: id={}", id);

        json result = _bookingService.deleteDesk(id);

        if (result.contains("status") && result["status"] == "error") {
            std::string message = result.contains("message")
                                      ? result["message"].get<std::string>()
                                      : "Error deleting desk";
            return errorResponse(400, message);
        }

        return successResponse(result);
    });
}
