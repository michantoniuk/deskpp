#ifndef BOOKING_CONTROLLER_H
#define BOOKING_CONTROLLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../service/booking_service.h"

using json = nlohmann::json;

class BookingController {
public:
    BookingController(BookingService &bookingService);

    crow::response getBuildings(const crow::request &req);

    crow::response getDesks(const crow::request &req);

    crow::response getBookings(const crow::request &req);

    crow::response addBooking(const crow::request &req);

    crow::response cancelBooking(int bookingId);

private:
    BookingService &_bookingService;

    json parseJson(const std::string &body);

    crow::response errorResponse(int statusCode, const std::string &message);

    crow::response successResponse(const json &data);

    std::optional<json> validateRequest(const crow::request &req,
                                        const std::vector<std::string> &requiredFields);
};

#endif // BOOKING_CONTROLLER_H
