#include "desk.h"
#include <algorithm>

Desk::Desk() : _id(0), _floorNumber(0) {
}

Desk::Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber)
    : _id(id), _deskId(deskId), _buildingId(buildingId), _floorNumber(floorNumber) {
}

// Getters
int Desk::getId() const {
    return _id;
}

std::string Desk::getDeskId() const {
    return _deskId;
}

std::string Desk::getBuildingId() const {
    return _buildingId;
}

int Desk::getFloorNumber() const {
    return _floorNumber;
}

// Setters
void Desk::setId(int id) {
    _id = id;
}

void Desk::setDeskId(const std::string &deskId) {
    _deskId = deskId;
}

void Desk::setBuildingId(const std::string &buildingId) {
    _buildingId = buildingId;
}

void Desk::setFloorNumber(int floorNumber) {
    _floorNumber = floorNumber;
}

// Simplified booking methods
bool Desk::isAvailable() const {
    return _bookings.empty();
}

bool Desk::isAvailableOn(const QDate &date) const {
    return !std::any_of(_bookings.begin(), _bookings.end(),
                        [&date](const Booking &booking) {
                            return booking.containsDate(date);
                        });
}

bool Desk::isAvailableForPeriod(const QDate &dateFrom, const QDate &dateTo) const {
    return !hasOverlappingBooking(dateFrom, dateTo);
}

bool Desk::hasOverlappingBooking(const QDate &dateFrom, const QDate &dateTo) const {
    for (const auto &booking: _bookings) {
        if (booking.overlapsWithPeriod(dateFrom, dateTo)) {
            return true;
        }
    }
    return false;
}

const std::vector<Booking> &Desk::getBookings() const {
    return _bookings;
}

Booking Desk::getBookingForDate(const QDate &date) const {
    auto it = std::find_if(_bookings.begin(), _bookings.end(),
                           [&date](const Booking &booking) {
                               return booking.containsDate(date);
                           });

    if (it != _bookings.end()) {
        return *it;
    }

    // Return default booking if not found
    return Booking();
}

std::vector<Booking> Desk::getBookingsAfterDate(const QDate &date) const {
    std::vector<Booking> result;

    std::copy_if(_bookings.begin(), _bookings.end(), std::back_inserter(result),
                 [&date](const Booking &booking) {
                     return booking.getDateFrom() >= date;
                 });

    // Sort by start date
    std::sort(result.begin(), result.end(),
              [](const Booking &a, const Booking &b) {
                  return a.getDateFrom() < b.getDateFrom();
              });

    return result;
}

std::vector<Booking> Desk::getBookingsContainingDate(const QDate &date) const {
    std::vector<Booking> result;

    std::copy_if(_bookings.begin(), _bookings.end(), std::back_inserter(result),
                 [&date](const Booking &booking) {
                     return booking.containsDate(date);
                 });

    return result;
}

void Desk::addBooking(const QDate &dateFrom, const QDate &dateTo, int bookingId) {
    Booking booking(bookingId, _id, 0, dateFrom, dateTo);
    addBooking(booking);
}

void Desk::addBooking(const Booking &booking) {
    _bookings.push_back(booking);
    sortBookings();
}

void Desk::cancelBooking(int bookingId) {
    if (bookingId <= 0) {
        return;
    }

    auto it = std::remove_if(_bookings.begin(), _bookings.end(),
                             [bookingId](const Booking &booking) {
                                 return booking.getId() == bookingId;
                             });

    if (it != _bookings.end()) {
        _bookings.erase(it, _bookings.end());
    }
}

void Desk::cancelAllBookings() {
    _bookings.clear();
}

void Desk::sortBookings() {
    std::sort(_bookings.begin(), _bookings.end(),
              [](const Booking &a, const Booking &b) {
                  return a.getDateFrom() < b.getDateFrom();
              });
}
