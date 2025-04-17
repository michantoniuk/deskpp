#include "desk.h"
#include <algorithm>

Desk::Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber,
           int locationX, int locationY)
    : Entity(id), _deskId(deskId), _buildingId(buildingId), _floorNumber(floorNumber),
      _locationX(locationX), _locationY(locationY) {
}

json Desk::toJson() const {
    return {
        {"id", getId()},
        {"deskId", _deskId},
        {"buildingId", _buildingId},
        {"floorNumber", _floorNumber},
        {"locationX", _locationX},
        {"locationY", _locationY}
    };
}

std::string Desk::toString() const {
    return "Desk: " + _deskId + " (ID: " + std::to_string(getId()) +
           ", loc: " + std::to_string(_locationX) + "," + std::to_string(_locationY) + ")";
}

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

    return (it != _bookings.end()) ? *it : Booking();
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
    Booking booking(bookingId, getId(), 0, dateFrom, dateTo);
    addBooking(booking);
}

void Desk::addBooking(const Booking &booking) {
    _bookings.push_back(booking);
    sortBookings();
}

void Desk::cancelBooking(int bookingId) {
    if (bookingId <= 0) return;

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
