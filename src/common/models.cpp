#include "models.h"
#include <algorithm>

// User implementation
User::User(int id, const std::string &username, const std::string &email, const std::string &fullName)
    : Entity(id), _username(username), _email(email), _fullName(fullName) {
}

json User::toJson() const {
    return {
        {"id", getId()},
        {"username", _username},
        {"email", _email},
        {"fullName", _fullName}
    };
}

std::string User::toString() const {
    return "User: " + _username + " (ID: " + std::to_string(getId()) + ")";
}

// Building implementation
Building::Building(int id, const std::string &name, const std::string &address)
    : Entity(id), _name(name), _address(address) {
}

json Building::toJson() const {
    return {
        {"id", getId()},
        {"name", _name},
        {"address", _address}
    };
}

std::string Building::toString() const {
    return "Building: " + _name + " (ID: " + std::to_string(getId()) + ")";
}

// Desk implementation
Desk::Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber)
    : Entity(id), _deskId(deskId), _buildingId(buildingId), _floorNumber(floorNumber) {
}

Desk::Desk(int id, const std::string &deskId, int buildingId, int floorNumber)
    : Entity(id), _deskId(deskId), _buildingId(std::to_string(buildingId)), _floorNumber(floorNumber) {
}

json Desk::toJson() const {
    return {
        {"id", getId()},
        {"deskId", _deskId},
        {"buildingId", _buildingId},
        {"floorNumber", _floorNumber}
    };
}

std::string Desk::toString() const {
    return "Desk: " + _deskId + " (ID: " + std::to_string(getId()) + ")";
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

// Booking implementation
Booking::Booking(int id, int deskId, int userId, const QDate &dateFrom, const QDate &dateTo)
    : Entity(id), _deskId(deskId), _userId(userId), _dateFrom(dateFrom), _dateTo(dateTo) {
}

Booking::Booking(int id, int deskId, int userId, const std::string &dateFrom, const std::string &dateTo)
    : Entity(id), _deskId(deskId), _userId(userId) {
    setDateFrom(dateFrom);
    setDateTo(dateTo);
}

bool Booking::containsDate(const QDate &date) const {
    return date >= _dateFrom && date <= _dateTo;
}

bool Booking::containsDate(const std::string &date) const {
    QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
    return containsDate(qdate);
}

bool Booking::overlapsWithPeriod(const QDate &dateFrom, const QDate &dateTo) const {
    // Special cases - consecutive bookings
    if (dateFrom == _dateTo || dateTo == _dateFrom) {
        return false;
    }

    // General case - check for overlap
    return !(_dateTo < dateFrom || _dateFrom > dateTo);
}

bool Booking::overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const {
    QDate from = QDate::fromString(QString::fromStdString(dateFrom), "yyyy-MM-dd");
    QDate to = QDate::fromString(QString::fromStdString(dateTo), "yyyy-MM-dd");
    return overlapsWithPeriod(from, to);
}

Booking Booking::fromJson(const json &j) {
    Booking booking;

    // Parse ID fields
    if (j.contains("id") && !j["id"].is_null())
        booking.setId(j["id"].get<int>());

    // Parse desk ID with fallback to alternate field name
    if (j.contains("deskId") && !j["deskId"].is_null())
        booking.setDeskId(j["deskId"].get<int>());
    else if (j.contains("desk_id") && !j["desk_id"].is_null())
        booking.setDeskId(j["desk_id"].get<int>());

    // Parse user ID with fallback to alternate field name
    if (j.contains("userId") && !j["userId"].is_null())
        booking.setUserId(j["userId"].get<int>());
    else if (j.contains("user_id") && !j["user_id"].is_null())
        booking.setUserId(j["user_id"].get<int>());

    // Parse date fields with fallbacks to alternate field names
    if (j.contains("dateFrom") && !j["dateFrom"].is_null()) {
        booking.setDateFrom(j["dateFrom"].get<std::string>());
    } else if (j.contains("date") && !j["date"].is_null()) {
        booking.setDateFrom(j["date"].get<std::string>());
    }

    if (j.contains("dateTo") && !j["dateTo"].is_null()) {
        booking.setDateTo(j["dateTo"].get<std::string>());
    } else if (j.contains("date_to") && !j["date_to"].is_null()) {
        booking.setDateTo(j["date_to"].get<std::string>());
    } else if (booking.getDateFrom().isValid()) {
        // Default to same day booking if dateTo is missing but dateFrom is valid
        booking.setDateTo(booking.getDateFrom());
    }

    return booking;
}

json Booking::toJson() const {
    return {
        {"id", getId()},
        {"deskId", _deskId},
        {"userId", _userId},
        {"dateFrom", getDateFromString()},
        {"dateTo", getDateToString()}
    };
}

std::string Booking::toString() const {
    return "Booking: " + std::to_string(_deskId) + " (ID: " + std::to_string(getId()) +
           ", from " + getDateFromString() + " to " + getDateToString() + ")";
}

std::string Booking::getDateFromString() const {
    return _dateFrom.toString("yyyy-MM-dd").toStdString();
}

std::string Booking::getDateToString() const {
    return _dateTo.toString("yyyy-MM-dd").toStdString();
}

void Booking::setDateFrom(const std::string &dateFrom) {
    _dateFrom = QDate::fromString(QString::fromStdString(dateFrom), "yyyy-MM-dd");
}

void Booking::setDateTo(const std::string &dateTo) {
    _dateTo = QDate::fromString(QString::fromStdString(dateTo), "yyyy-MM-dd");
}

bool Booking::isDateBefore(const std::string &date1, const std::string &date2) const {
    return date1 < date2;
}

bool Booking::isDateAfter(const std::string &date1, const std::string &date2) const {
    return date1 > date2;
}

bool Booking::isDateEqual(const std::string &date1, const std::string &date2) const {
    return date1 == date2;
}
