#include "booking_service.h"
#include "../util/logger.h"
#include "../util/date_helper.h"

BookingService::BookingService(BookingRepository &bookingRepo,
                               DeskRepository &deskRepo,
                               BuildingRepository &buildingRepo)
    : _bookingRepo(bookingRepo),
      _deskRepo(deskRepo),
      _buildingRepo(buildingRepo) {
}

json BookingService::getAllBuildings() {
    auto buildings = _buildingRepo.getAllBuildings();

    json buildingsArray = json::array();
    for (const auto &building: buildings) {
        json buildingJson;
        buildingJson["id"] = building.getId();
        buildingJson["name"] = building.getName();
        buildingJson["address"] = building.getAddress();
        buildingsArray.push_back(buildingJson);
    }

    return {{"status", "success"}, {"buildings", buildingsArray}};
}

json BookingService::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;

    if (buildingId > 0) {
        desks = _deskRepo.getDesksByBuilding(buildingId);
    } else {
        desks = _deskRepo.getAllDesks();
    }

    json desksArray = json::array();
    for (const auto &desk: desks) {
        json deskJson;
        deskJson["id"] = desk.getId();
        deskJson["deskId"] = desk.getDeskId();
        deskJson["buildingId"] = desk.getBuildingId();
        deskJson["floorNumber"] = desk.getFloorNumber();

        // Get active bookings for this desk
        auto bookings = _bookingRepo.getActiveBookingsForDesk(desk.getId());

        bool hasBookings = !bookings.empty();
        deskJson["booked"] = hasBookings;
        deskJson["available"] = !hasBookings;

        // Add bookings to the JSON response
        json bookingsArray = json::array();
        for (const auto &booking: bookings) {
            json bookingJson;
            bookingJson["id"] = booking.getId();
            bookingJson["userId"] = booking.getUserId();
            bookingJson["dateFrom"] = booking.getDateFrom(); // Already a string
            bookingJson["dateTo"] = booking.getDateTo(); // Already a string
            bookingsArray.push_back(bookingJson);
        }

        if (hasBookings) {
            deskJson["bookings"] = bookingsArray;

            // For backward compatibility, add first booking's dates
            if (!bookings.empty()) {
                deskJson["bookingDateFrom"] = bookings[0].getDateFrom();
                deskJson["bookingDateTo"] = bookings[0].getDateTo();
            }
        }

        desksArray.push_back(deskJson);
    }

    return {{"status", "success"}, {"desks", desksArray}};
}

json BookingService::getBookingsForDesk(int deskId, const std::string &date) {
    auto bookings = _bookingRepo.getBookingsForDate(deskId, date);

    json bookingsArray = json::array();
    for (const auto &booking: bookings) {
        json bookingJson;
        bookingJson["id"] = booking.getId();
        bookingJson["deskId"] = booking.getDeskId();
        bookingJson["userId"] = booking.getUserId();
        bookingJson["dateFrom"] = booking.getDateFrom(); // Already a string
        bookingJson["dateTo"] = booking.getDateTo(); // Already a string
        bookingsArray.push_back(bookingJson);
    }

    return {{"status", "success"}, {"bookings", bookingsArray}};
}

json BookingService::getBookingsForDeskInRange(int deskId,
                                               const std::string &dateFrom,
                                               const std::string &dateTo) {
    auto bookings = _bookingRepo.getBookingsInRange(deskId, dateFrom, dateTo);

    json bookingsArray = json::array();
    for (const auto &booking: bookings) {
        json bookingJson;
        bookingJson["id"] = booking.getId();
        bookingJson["deskId"] = booking.getDeskId();
        bookingJson["userId"] = booking.getUserId();
        bookingJson["dateFrom"] = booking.getDateFrom(); // Already a string
        bookingJson["dateTo"] = booking.getDateTo(); // Already a string
        bookingsArray.push_back(bookingJson);
    }

    return {{"status", "success"}, {"bookings", bookingsArray}};
}

json BookingService::addBooking(int deskId, int userId,
                                const std::string &dateFrom,
                                const std::string &dateTo) {
    // Validate input data
    if (!validateBookingDates(dateFrom, dateTo)) {
        return {{"status", "error"}, {"message", "Invalid date range"}};
    }

    // Check if desk exists
    auto desk = _deskRepo.getDeskById(deskId);
    if (!desk) {
        return {{"status", "error"}, {"message", "Desk does not exist"}};
    }

    // Check if booking is allowed
    if (!isBookingAllowed(deskId, dateFrom, dateTo)) {
        return {{"status", "error"}, {"message", "Desk is already booked in the selected period"}};
    }

    // Add booking
    auto bookingId = _bookingRepo.addBooking(deskId, userId, dateFrom, dateTo);
    if (!bookingId) {
        return {{"status", "error"}, {"message", "Error adding booking"}};
    }

    return {
        {"status", "success"},
        {
            "booking", {
                {"id", *bookingId},
                {"deskId", deskId},
                {"userId", userId},
                {"dateFrom", dateFrom},
                {"dateTo", dateTo}
            }
        }
    };
}

json BookingService::cancelBooking(int bookingId) {
    // Check if booking exists
    auto booking = _bookingRepo.getBookingById(bookingId);
    if (!booking) {
        return {{"status", "error"}, {"message", "Booking does not exist"}};
    }

    // Cancel booking
    bool success = _bookingRepo.cancelBooking(bookingId);
    if (!success) {
        return {{"status", "error"}, {"message", "Error cancelling booking"}};
    }

    return {{"status", "success"}, {"message", "Booking has been canceled"}};
}

bool BookingService::validateBookingDates(const std::string &dateFrom, const std::string &dateTo) {
    // Check if dates are in valid format
    if (!DateHelper::isValidDate(dateFrom) || !DateHelper::isValidDate(dateTo)) {
        return false;
    }

    // Check if date range is valid (from <= to)
    if (DateHelper::isDateBefore(dateTo, dateFrom)) {
        return false;
    }

    // Check if dates are not in the past
    std::string today = DateHelper::getCurrentDate();
    if (DateHelper::isDateBefore(dateFrom, today)) {
        return false;
    }

    return true;
}

bool BookingService::isBookingAllowed(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    // Check for overlapping bookings
    return !_bookingRepo.hasOverlappingBooking(deskId, dateFrom, dateTo);
}
