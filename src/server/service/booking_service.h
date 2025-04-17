#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "service.h"
#include <string>
#include <nlohmann/json.hpp>
#include "../repository/building_repository.h"
#include "../repository/desk_repository.h"
#include "../repository/booking_repository.h"

using json = nlohmann::json;

class BookingService : public Service<Booking> {
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

    json addBuilding(const std::string &name, const std::string &address);

    json updateBuilding(int id, const std::string &name, const std::string &address);

    json deleteBuilding(int id);

    json addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX = 0, int locationY = 0);

    json updateDesk(int id, const json &deskData);

    json deleteDesk(int id);

private:
    BuildingRepository &_buildingRepo;
    DeskRepository &_deskRepo;
    BookingRepository &_bookingRepo;

    // Helper method for Building entities (since Service<Booking>::entityListToJson works only with Booking entities)
    json buildingListToJson(const std::vector<Building> &buildings, const std::string &key);

    // Validation helpers
    bool validateBookingDates(const std::string &dateFrom, const std::string &dateTo);

    bool isBookingAllowed(int deskId, const std::string &dateFrom, const std::string &dateTo);
};

#endif // BOOKING_SERVICE_H
