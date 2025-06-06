#include "desk.h"
#include <algorithm>

Desk::Desk(int id, const std::string &name, int buildingId, int floor)
    : Entity(id), _name(name), _buildingId(buildingId), _floor(floor) {
}

json Desk::toJson() const {
    return {
        {"id", getId()},
        {"name", _name},
        {"buildingId", _buildingId},
        {"floor", _floor}
    };
}

std::string Desk::toString() const {
    return "Desk: " + _name + " (ID: " + std::to_string(getId()) + ", Floor: " + std::to_string(_floor) + ")";
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
    return std::any_of(_bookings.begin(), _bookings.end(),
                       [&](const Booking &booking) {
                           return booking.overlapsWithPeriod(dateFrom, dateTo);
                       });
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

void Desk::sortBookings() {
    std::sort(_bookings.begin(), _bookings.end(),
              [](const Booking &a, const Booking &b) {
                  return a.getDateFrom() < b.getDateFrom();
              });
}
