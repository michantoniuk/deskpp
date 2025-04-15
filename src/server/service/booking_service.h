#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "base_service.h"
#include <string>
#include <nlohmann/json.hpp>
#include "../repository/building_repository.h"
#include "../repository/desk_repository.h"
#include "../repository/booking_repository.h"

using json = nlohmann::json;

/**
 * Service layer for booking operations
 */
class BookingService : public BaseService {
public:
    BookingService(BuildingRepository &buildingRepository,
                   DeskRepository &deskRepository,
                   BookingRepository &bookingRepository);

    json getAllBuildings();

    json getDesksByBuilding(int buildingId = -1);

    json getBookingsForDesk(int deskId, const std::string &date);

    json getBookingsForDeskInRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    json addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    json cancelBooking(int bookingId);

private:
    BuildingRepository &_buildingRepository;
    DeskRepository &_deskRepository;
    BookingRepository &_bookingRepository;

    // Validation helpers
    bool validateBookingDates(const std::string &dateFrom, const std::string &dateTo);

    bool isBookingAllowed(int deskId, const std::string &dateFrom, const std::string &dateTo);
};

#endif // BOOKING_SERVICE_H
