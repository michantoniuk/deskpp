#ifndef SERVER_BOOKING_CONTROLLER_H
#define SERVER_BOOKING_CONTROLLER_H

#include "base_controller.h"
#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include "../service/booking_service.h"

using json = nlohmann::json;

/**
 * Controller for booking-related API endpoints
 */
class BookingController : public BaseController {
public:
    explicit BookingController(BookingService &bookingService);

    crow::response getBuildings(const crow::request &req);

    crow::response getDesks(const crow::request &req);

    crow::response getBookings(const crow::request &req);

    crow::response addBooking(const crow::request &req);

    crow::response cancelBooking(int bookingId);

private:
    BookingService &_bookingService;
};

#endif // SERVER_BOOKING_CONTROLLER_H
