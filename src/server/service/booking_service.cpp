#include "booking_service.h"
#include <regex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "../util/logger.h"

BookingService::BookingService(BuildingRepository &buildingRepository,
                               DeskRepository &deskRepository,
                               BookingRepository &bookingRepository)
    : _buildingRepository(buildingRepository),
      _deskRepository(deskRepository),
      _bookingRepository(bookingRepository) {
}

json BookingService::getAllBuildings() {
    auto buildings = _buildingRepository.findAll();
    return entityListToJson(buildings, "buildings");
}

json BookingService::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;

    if (buildingId > 0) {
        desks = _deskRepository.findByBuildingId(buildingId);
    } else {
        desks = _deskRepository.findAll();
    }

    json desksArray = json::array();
    for (const auto &desk: desks) {
        json deskJson = desk.toJson();

        // Get active bookings for this desk
        auto bookings = _bookingRepository.findByDeskId(desk.getId());

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
    auto bookings = _bookingRepository.findBy([deskId, &date](const Booking &booking) {
        return booking.getDeskId() == deskId && booking.containsDate(date);
    });

    return entityListToJson(bookings, "bookings");
}

json BookingService::getBookingsForDeskInRange(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    auto bookings = _bookingRepository.findByDateRange(deskId, dateFrom, dateTo);
    return entityListToJson(bookings, "bookings");
}

json BookingService::addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo) {
    // Validate input data
    if (!validateBookingDates(dateFrom, dateTo)) {
        return errorResponse("Invalid date range");
    }

    // Check if desk exists
    auto desk = _deskRepository.findById(deskId);
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
    Booking createdBooking = _bookingRepository.add(booking);

    // Check if booking was created successfully
    if (createdBooking.getId() <= 0) {
        return errorResponse("Error adding booking");
    }

    return successResponse({{"booking", createdBooking.toJson()}});
}

json BookingService::cancelBooking(int bookingId) {
    // Check if booking exists
    auto booking = _bookingRepository.findById(bookingId);
    if (!booking) {
        return errorResponse("Booking does not exist");
    }

    // Cancel booking
    bool success = _bookingRepository.remove(bookingId);
    if (!success) {
        return errorResponse("Error cancelling booking");
    }

    return successResponse({{"message", "Booking has been canceled"}});
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
    return !_bookingRepository.hasOverlappingBooking(deskId, dateFrom, dateTo);
}
