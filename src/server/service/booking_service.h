#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "service.h"
#include "../repository/building_repository.h"
#include "../repository/desk_repository.h"
#include "../repository/booking_repository.h"

class BookingService : public Service<Booking> {
public:
    BookingService(BuildingRepository &buildingRepository,
                   DeskRepository &deskRepository,
                   BookingRepository &bookingRepository);

    // Building operations
    json getAllBuildings();

    json addBuilding(const std::string &name, const std::string &address);

    json updateBuilding(int id, const std::string &name, const std::string &address);

    json deleteBuilding(int id);

    // Desk operations
    json getDesksByBuilding(int buildingId = -1);

    json addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX = 0, int locationY = 0);

    json updateDesk(int id, const json &deskData);

    json deleteDesk(int id);

    // Booking operations
    json getBookingsForDesk(int deskId, const std::string &date);

    json getBookingsForDeskInRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    json addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    json cancelBooking(int bookingId);

private:
    BuildingRepository &_buildingRepo;
    DeskRepository &_deskRepo;
    BookingRepository &_bookingRepo;
};

#endif // BOOKING_SERVICE_H
