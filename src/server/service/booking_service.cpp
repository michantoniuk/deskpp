#include "booking_service.h"

#include <set>

BookingService::BookingService(BuildingRepository &buildingRepository, DeskRepository &deskRepository,
                               BookingRepository &bookingRepository)
    : Service<Booking>(bookingRepository),
      _buildingRepo(buildingRepository),
      _deskRepo(deskRepository),
      _bookingRepo(bookingRepository) {
}

json BookingService::getAllBuildings() {
    auto buildings = _buildingRepo.findAll();
    json array = json::array();

    for (const auto &building: buildings) {
        auto buildingJson = building.toJson();
        LOG_INFO("Server sending building {} with {} floors",
                 building.getId(), building.getNumFloors());
        array.push_back(buildingJson);
    }

    return successResponse({{"buildings", array}});
}

json BookingService::getAllDesks() {
    auto desks = _deskRepo.findAll();
    json array = json::array();

    for (const auto &desk: desks) {
        json deskJson = desk.toJson();

        // Add bookings
        auto bookings = _bookingRepo.findByDeskId(desk.getId());
        json bookingsArray = json::array();
        for (const auto &booking: bookings) {
            bookingsArray.push_back(booking.toJson());
        }
        deskJson["bookings"] = bookingsArray;
        array.push_back(deskJson);
    }

    return successResponse({{"desks", array}});
}

json BookingService::getDesksByBuilding(int buildingId) {
    auto desks = _deskRepo.findByBuildingId(buildingId);
    json array = json::array();

    for (const auto &desk: desks) {
        json deskJson = desk.toJson();

        // Add bookings
        auto bookings = _bookingRepo.findByDeskId(desk.getId());
        json bookingsArray = json::array();
        for (const auto &booking: bookings) {
            bookingsArray.push_back(booking.toJson());
        }
        deskJson["bookings"] = bookingsArray;
        array.push_back(deskJson);
    }

    return successResponse({{"desks", array}});
}

json BookingService::getBookingsForDesk(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    auto bookings = _bookingRepo.findByDateRange(deskId, dateFrom, dateTo);
    json array = json::array();
    for (const auto &booking: bookings) {
        array.push_back(booking.toJson());
    }
    return successResponse({{"bookings", array}});
}

json BookingService::addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo) {
    // Check if desk exists
    if (!_deskRepo.findById(deskId)) {
        return errorResponse("Desk not found");
    }

    // Check for overlapping bookings
    if (_bookingRepo.hasOverlappingBooking(deskId, dateFrom, dateTo)) {
        return errorResponse("Desk already booked for this period");
    }

    // Create booking
    Booking booking;
    booking.setDeskId(deskId);
    booking.setUserId(userId);
    booking.setDateFrom(dateFrom);
    booking.setDateTo(dateTo);
    Booking created = _repository.add(booking);
    return successResponse({{"booking", created.toJson()}});
}

json BookingService::cancelBooking(int bookingId) {
    // Check if exists
    if (!_repository.findById(bookingId)) {
        return errorResponse("Booking not found");
    }
    _repository.remove(bookingId);
    return successResponse({{"message", "Booking canceled"}});
}

json BookingService::getDesksByBuildingAndFloor(int buildingId, int floor) {
    auto desks = _deskRepo.findByBuildingId(buildingId);
    json array = json::array();

    for (const auto &desk: desks) {
        // Filter by floor
        if (desk.getFloor() != floor) {
            continue;
        }

        json deskJson = desk.toJson();

        // Add bookings
        auto bookings = _bookingRepo.findByDeskId(desk.getId());
        json bookingsArray = json::array();
        for (const auto &booking: bookings) {
            bookingsArray.push_back(booking.toJson());
        }
        deskJson["bookings"] = bookingsArray;
        array.push_back(deskJson);
    }

    return successResponse({{"desks", array}});
}

json BookingService::getFloorsByBuilding(int buildingId) {
    // Check if building exists
    auto buildingOpt = _buildingRepo.findById(buildingId);
    if (!buildingOpt) {
        return errorResponse("Building not found");
    }

    Building building = *buildingOpt;
    auto floors = building.getFloors();

    json floorsArray = json::array();
    for (int floor: floors) {
        floorsArray.push_back(floor);
    }

    return successResponse({{"floors", floorsArray}});
}
