#include "booking_service.h"

BookingService::BookingService(BuildingRepository &buildingRepository,
                               DeskRepository &deskRepository,
                               BookingRepository &bookingRepository)
    : Service<Booking>(bookingRepository),
      _buildingRepo(buildingRepository),
      _deskRepo(deskRepository),
      _bookingRepo(bookingRepository) {
}

// Building operations
json BookingService::getAllBuildings() {
    auto buildings = _buildingRepo.findAll();

    json array = json::array();
    for (const auto &building: buildings) {
        array.push_back(building.toJson());
    }

    return successResponse({{"buildings", array}});
}

json BookingService::addBuilding(const std::string &name, const std::string &address) {
    // Check for duplicate
    if (_buildingRepo.findByName(name)) {
        return errorResponse("Building with this name already exists");
    }

    // Create building
    Building building;
    building.setName(name);
    building.setAddress(address);

    Building created = _buildingRepo.add(building);

    return successResponse({{"building", created.toJson()}});
}

json BookingService::updateBuilding(int id, const std::string &name, const std::string &address) {
    // Check if exists
    auto building = _buildingRepo.findById(id);
    if (!building) {
        return errorResponse("Building not found");
    }

    // Update
    building->setName(name);
    building->setAddress(address);

    _buildingRepo.update(*building);

    return successResponse({{"building", building->toJson()}});
}

json BookingService::deleteBuilding(int id) {
    // Check if exists
    if (!_buildingRepo.findById(id)) {
        return errorResponse("Building not found");
    }

    _buildingRepo.remove(id);

    return successResponse({{"message", "Building deleted"}});
}

// Desk operations
json BookingService::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;

    if (buildingId > 0) {
        desks = _deskRepo.findByBuildingId(buildingId);
    } else {
        desks = _deskRepo.findAll();
    }

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

json BookingService::addDesk(const std::string &deskId, int buildingId, int floorNumber, int locationX, int locationY) {
    // Check if building exists
    if (!_buildingRepo.findById(buildingId)) {
        return errorResponse("Building not found");
    }

    // Create desk
    Desk desk;
    desk.setDeskId(deskId);
    desk.setBuildingId(std::to_string(buildingId));
    desk.setFloorNumber(floorNumber);
    desk.setLocationX(locationX);
    desk.setLocationY(locationY);

    Desk created = _deskRepo.add(desk);

    return successResponse({{"desk", created.toJson()}});
}

json BookingService::updateDesk(int id, const json &deskData) {
    // Check if exists
    auto desk = _deskRepo.findById(id);
    if (!desk) {
        return errorResponse("Desk not found");
    }

    // Update fields
    if (deskData.contains("deskId")) {
        desk->setDeskId(deskData["deskId"].get<std::string>());
    }

    if (deskData.contains("buildingId")) {
        desk->setBuildingId(std::to_string(deskData["buildingId"].get<int>()));
    }

    if (deskData.contains("floorNumber")) {
        desk->setFloorNumber(deskData["floorNumber"].get<int>());
    }

    if (deskData.contains("locationX")) {
        desk->setLocationX(deskData["locationX"].get<int>());
    }

    if (deskData.contains("locationY")) {
        desk->setLocationY(deskData["locationY"].get<int>());
    }

    _deskRepo.update(*desk);

    return successResponse({{"desk", desk->toJson()}});
}

json BookingService::deleteDesk(int id) {
    // Check if exists
    if (!_deskRepo.findById(id)) {
        return errorResponse("Desk not found");
    }

    _deskRepo.remove(id);

    return successResponse({{"message", "Desk deleted"}});
}

// Booking operations
json BookingService::getBookingsForDesk(int deskId, const std::string &date) {
    auto bookings = _bookingRepo.findByDate(date);

    // Filter by desk ID
    std::vector<Booking> filteredBookings;
    for (const auto &booking: bookings) {
        if (booking.getDeskId() == deskId) {
            filteredBookings.push_back(booking);
        }
    }

    json array = json::array();
    for (const auto &booking: filteredBookings) {
        array.push_back(booking.toJson());
    }

    return successResponse({{"bookings", array}});
}

json BookingService::getBookingsForDeskInRange(int deskId, const std::string &dateFrom, const std::string &dateTo) {
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
