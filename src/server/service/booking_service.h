#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include <string>
#include <nlohmann/json.hpp>
#include "../repository/booking_repository.h"
#include "../repository/desk_repository.h"
#include "../repository/building_repository.h"

using json = nlohmann::json;

class BookingService {
public:
    BookingService(BookingRepository &bookingRepo,
                   DeskRepository &deskRepo,
                   BuildingRepository &buildingRepo);

    json getAllBuildings();

    json getDesksByBuilding(int buildingId = -1);

    json getBookingsForDesk(int deskId, const std::string &date);

    json getBookingsForDeskInRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    json addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    json cancelBooking(int bookingId);

private:
    BookingRepository &_bookingRepo;
    DeskRepository &_deskRepo;
    BuildingRepository &_buildingRepo;

    bool validateBookingDates(const std::string &dateFrom, const std::string &dateTo);

    bool isBookingAllowed(int deskId, const std::string &dateFrom, const std::string &dateTo);
};

#endif // BOOKING_SERVICE_H
