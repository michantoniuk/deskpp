#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H

#include "controller.h"
#include "../../service/booking_service.h"

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

#endif
