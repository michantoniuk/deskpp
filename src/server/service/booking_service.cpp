#include "booking_service.h"
#include <regex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "common/logger.h"

BookingService::BookingService(BuildingRepository &buildingRepository,
                               DeskRepository &deskRepository,
                               BookingRepository &bookingRepository)
    : Service<Booking>(bookingRepository),
      _buildingRepo(buildingRepository),
      _deskRepo(deskRepository),
      _bookingRepo(bookingRepository) {
}

json BookingService::getAllBuildings() {
    auto buildings = _buildingRepo.findAll();
    return buildingListToJson(buildings, "buildings");
}

json BookingService::buildingListToJson(const std::vector<Building> &buildings, const std::string &key) {
    json array = json::array();
    for (const auto &building: buildings) {
        array.push_back(building.toJson());
    }

    return successResponse({{key, array}});
}

json BookingService::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;

    if (buildingId > 0) {
        desks = _deskRepo.findByBuildingId(buildingId);
    } else {
        desks = _deskRepo.findAll();
    }

    json desksArray = json::array();
    for (const auto &desk: desks) {
        json deskJson = desk.toJson();

        // Get active bookings for this desk
        auto bookings = _bookingRepo.findByDeskId(desk.getId());

        bool hasBookings = !bookings.empty();
        deskJson["booked"] = hasBookings;
        deskJson["available"] = !hasBookings;

        // Add bookings to the JSON response
        json bookingsArray = json::array();
        for (const auto &booking: bookings) {
            bookingsArray.push_back(booking.toJson());
        }

        if (hasBookings) {
            deskJson["bookings"] = bookingsArray;

            // For backward compatibility, add first booking's dates
            if (!bookings.empty()) {
                deskJson["bookingDateFrom"] = bookings[0].getDateFromString();
                deskJson["bookingDateTo"] = bookings[0].getDateToString();
            }
        }

        desksArray.push_back(deskJson);
    }

    return successResponse({{"desks", desksArray}});
}

json BookingService::getBookingsForDesk(int deskId, const std::string &date) {
    auto bookings = _bookingRepo.findBy([deskId, &date](const Booking &booking) {
        return booking.getDeskId() == deskId && booking.containsDate(date);
    });

    return entityListToJson(bookings, "bookings");
}

json BookingService::getBookingsForDeskInRange(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    auto bookings = _bookingRepo.findByDateRange(deskId, dateFrom, dateTo);
    return entityListToJson(bookings, "bookings");
}

json BookingService::addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo) {
    // Validate input data
    if (!validateBookingDates(dateFrom, dateTo)) {
        return errorResponse("Invalid date range");
    }

    // Check if desk exists
    auto desk = _deskRepo.findById(deskId);
    if (!desk) {
        return errorResponse("Desk does not exist");
    }

    // Check if booking is allowed
    if (!isBookingAllowed(deskId, dateFrom, dateTo)) {
        return errorResponse("Desk is already booked in the selected period");
    }

    // Create booking
    Booking booking;
    booking.setDeskId(deskId);
    booking.setUserId(userId);
    booking.setDateFrom(dateFrom);
    booking.setDateTo(dateTo);

    // Add booking
    Booking createdBooking = _repository.add(booking);

    // Check if booking was created successfully
    if (createdBooking.getId() <= 0) {
        return errorResponse("Error adding booking");
    }

    return successResponse({{"booking", createdBooking.toJson()}});
}

json BookingService::cancelBooking(int bookingId) {
    return removeById(bookingId, "Booking has been canceled", "Booking does not exist");
}

bool BookingService::validateBookingDates(const std::string &dateFrom, const std::string &dateTo) {
    // Check if dates are in valid format (YYYY-MM-DD)
    std::regex dateRegex("^\\d{4}-\\d{2}-\\d{2}$");
    if (!std::regex_match(dateFrom, dateRegex) || !std::regex_match(dateTo, dateRegex)) {
        return false;
    }

    // Check if date range is valid (from <= to)
    if (dateFrom > dateTo) {
        return false;
    }

    // Get current date
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d");
    std::string today = oss.str();

    // Check if dates are not in the past
    if (dateFrom < today) {
        return false;
    }

    return true;
}

bool BookingService::isBookingAllowed(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    // Check for overlapping bookings
    return !_bookingRepo.hasOverlappingBooking(deskId, dateFrom, dateTo);
}

json BookingService::addBuilding(const std::string &name, const std::string &address) {
    // Check if building with same name already exists
    auto existingBuilding = _buildingRepo.findByName(name);
    if (existingBuilding) {
        return errorResponse("Building with this name already exists");
    }

    // Create building
    Building building;
    building.setName(name);
    building.setAddress(address);

    // Add building to repository
    Building createdBuilding = _buildingRepo.add(building);

    // Check if building was created successfully
    if (createdBuilding.getId() <= 0) {
        return errorResponse("Error adding building");
    }

    return successResponse({{"building", createdBuilding.toJson()}});
}

json BookingService::updateBuilding(int id, const std::string &name, const std::string &address) {
    // Check if building exists
    auto building = _buildingRepo.findById(id);
    if (!building) {
        return errorResponse("Building not found");
    }

    // Check if name is already taken by another building
    auto existingBuilding = _buildingRepo.findByName(name);
    if (existingBuilding && existingBuilding->getId() != id) {
        return errorResponse("Building with this name already exists");
    }

    // Update building
    Building updatedBuilding = *building;
    updatedBuilding.setName(name);
    updatedBuilding.setAddress(address);

    // Update in repository
    bool success = _buildingRepo.update(updatedBuilding);
    if (!success) {
        return errorResponse("Error updating building");
    }

    return successResponse({{"building", updatedBuilding.toJson()}});
}

json BookingService::deleteBuilding(int id) {
    // Check if building exists
    auto building = _buildingRepo.findById(id);
    if (!building) {
        return errorResponse("Building not found");
    }

    // Check if there are any desks in this building
    auto desks = _deskRepo.findByBuildingId(id);
    if (!desks.empty()) {
        return errorResponse("Cannot delete building with existing desks");
    }

    // Delete building
    bool success = _buildingRepo.remove(id);
    if (!success) {
        return errorResponse("Error deleting building");
    }

    return successResponse({{"message", "Building deleted successfully"}});
}

json BookingService::addDesk(const std::string &deskId, int buildingId, int floorNumber,
                             int locationX, int locationY) {
    // Check if building exists
    auto building = _buildingRepo.findById(buildingId);
    if (!building) {
        return errorResponse("Building not found");
    }

    // Check if desk ID is already taken
    auto existingDesk = _deskRepo.findByDeskNumber(deskId);
    if (existingDesk) {
        return errorResponse("Desk with this ID already exists");
    }

    // Create desk
    Desk desk;
    desk.setDeskId(deskId);
    desk.setBuildingId(std::to_string(buildingId));
    desk.setFloorNumber(floorNumber);
    desk.setLocationX(locationX);
    desk.setLocationY(locationY);

    // Add desk to repository
    Desk createdDesk = _deskRepo.add(desk);

    // Check if desk was created successfully
    if (createdDesk.getId() <= 0) {
        return errorResponse("Error adding desk");
    }

    return successResponse({{"desk", createdDesk.toJson()}});
}

json BookingService::updateDesk(int id, const json &deskData) {
    // Check if desk exists
    auto desk = _deskRepo.findById(id);
    if (!desk) {
        return errorResponse("Desk not found");
    }

    Desk updatedDesk = *desk;

    // Update desk fields if provided
    if (deskData.contains("deskId") && !deskData["deskId"].is_null()) {
        std::string newDeskId = deskData["deskId"].get<std::string>();

        // Check if new desk ID is already taken by another desk
        auto existingDesk = _deskRepo.findByDeskNumber(newDeskId);
        if (existingDesk && existingDesk->getId() != id) {
            return errorResponse("Desk with this ID already exists");
        }

        updatedDesk.setDeskId(newDeskId);
    }

    if (deskData.contains("buildingId") && !deskData["buildingId"].is_null()) {
        int buildingId = deskData["buildingId"].get<int>();

        // Check if building exists
        auto building = _buildingRepo.findById(buildingId);
        if (!building) {
            return errorResponse("Building not found");
        }

        updatedDesk.setBuildingId(std::to_string(buildingId));
    }

    if (deskData.contains("floorNumber") && !deskData["floorNumber"].is_null()) {
        updatedDesk.setFloorNumber(deskData["floorNumber"].get<int>());
    }

    if (deskData.contains("locationX") && !deskData["locationX"].is_null()) {
        updatedDesk.setLocationX(deskData["locationX"].get<int>());
    }

    if (deskData.contains("locationY") && !deskData["locationY"].is_null()) {
        updatedDesk.setLocationY(deskData["locationY"].get<int>());
    }

    // Update in repository
    bool success = _deskRepo.update(updatedDesk);
    if (!success) {
        return errorResponse("Error updating desk");
    }

    return successResponse({{"desk", updatedDesk.toJson()}});
}

json BookingService::deleteDesk(int id) {
    // Check if desk exists
    auto desk = _deskRepo.findById(id);
    if (!desk) {
        return errorResponse("Desk not found");
    }

    // Check if there are any bookings for this desk
    auto bookings = _bookingRepo.findByDeskId(id);
    if (!bookings.empty()) {
        return errorResponse("Cannot delete desk with existing bookings");
    }

    // Delete desk
    bool success = _deskRepo.remove(id);
    if (!success) {
        return errorResponse("Error deleting desk");
    }

    return successResponse({{"message", "Desk deleted successfully"}});
}
