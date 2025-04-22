#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "service.h"
#include "../repository/building_repository.h"
#include "../repository/desk_repository.h"
#include "../repository/booking_repository.h"

class BookingService : public Service<Booking> {
public:
    BookingService(BuildingRepository &buildingRepository, DeskRepository &deskRepository,
                   BookingRepository &bookingRepository);

    // Building operations
    json getAllBuildings();

    // Desk operations
    json getAllDesks();

    json getDesksByBuilding(int buildingId);

    // Booking operations
    json getBookingsForDesk(int deskId, const std::string &dateFrom, const std::string &dateTo);

    json addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    json cancelBooking(int bookingId);

    json getDesksByBuildingAndFloor(int buildingId, int floor);

    json getFloorsByBuilding(int buildingId);

private:
    BuildingRepository &_buildingRepo;
    DeskRepository &_deskRepo;
    BookingRepository &_bookingRepo;
};

#endif
