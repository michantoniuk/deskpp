// src/server/api/controller/admin_controller.h

#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H

#include "controller.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include "server/service/booking_service.h"

using json = nlohmann::json;

class AdminController : public Controller {
public:
    explicit AdminController(BookingService &bookingService);

    // Building management
    crow::response addBuilding(const crow::request &req);

    crow::response updateBuilding(int id, const crow::request &req);

    crow::response deleteBuilding(int id);

    // Desk management
    crow::response addDesk(const crow::request &req);

    crow::response updateDesk(int id, const crow::request &req);

    crow::response deleteDesk(int id);

private:
    BookingService &_bookingService;
};

#endif // ADMIN_CONTROLLER_H
