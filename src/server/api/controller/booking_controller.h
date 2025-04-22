#ifndef BOOKING_CONTROLLER_H
#define BOOKING_CONTROLLER_H

#include "controller.h"
#include "../../service/booking_service.h"

class BookingController : public Controller {
public:
    explicit BookingController(BookingService &bookingService);

    crow::response getBuildings(const crow::request &req);

    crow::response getDesks(const crow::request &req);

    crow::response getBookings(const crow::request &req);

    crow::response addBooking(const crow::request &req);

    crow::response cancelBooking(int bookingId);

    crow::response getFloorsByBuilding(int buildingId);

private:
    BookingService &_bookingService;
};

#endif
