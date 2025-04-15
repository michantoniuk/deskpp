#include "models.h"
#include <algorithm>

// User implementation
User::User(int id, const std::string &username, const std::string &email, const std::string &fullName)
    : _id(id), _username(username), _email(email), _fullName(fullName) {
}

User User::fromJson(const json &j) {
    User user;
    
    if (j.contains("id") && !j["id"].is_null())
        user.setId(j["id"].get<int>());
        
    if (j.contains("username") && !j["username"].is_null())
        user.setUsername(j["username"].get<std::string>());
        
    if (j.contains("email") && !j["email"].is_null())
        user.setEmail(j["email"].get<std::string>());
        
    if (j.contains("fullName") && !j["fullName"].is_null())
        user.setFullName(j["fullName"].get<std::string>());
        
    return user;
}

json User::toJson() const {
    return {
        {"id", _id},
        {"username", _username},
        {"email", _email},
        {"fullName", _fullName}
    };
}

// Booking implementation
Booking::Booking(int id, int deskId, int userId, const QDate& dateFrom, const QDate& dateTo)
    : _id(id), _deskId(deskId), _userId(userId), _dateFrom(dateFrom), _dateTo(dateTo) {
}

bool Booking::containsDate(const QDate& date) const {
    return date >= _dateFrom && date <= _dateTo;
}

bool Booking::overlapsWithPeriod(const QDate& dateFrom, const QDate& dateTo) const {
    // Special cases - consecutive bookings
    if (dateFrom == _dateTo || dateTo == _dateFrom) {
        return false;
    }
    
    // General case - check for overlap
    return !(_dateTo < dateFrom || _dateFrom > dateTo);
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
        std::string dateFromStr = j["dateFrom"].get<std::string>();
        QDate dateFrom = QDate::fromString(QString::fromStdString(dateFromStr), "yyyy-MM-dd");
        booking.setDateFrom(dateFrom);
    } else if (j.contains("date") && !j["date"].is_null()) {
        std::string dateStr = j["date"].get<std::string>();
        QDate date = QDate::fromString(QString::fromStdString(dateStr), "yyyy-MM-dd");
        booking.setDateFrom(date);
    }
    
    if (j.contains("dateTo") && !j["dateTo"].is_null()) {
        std::string dateToStr = j["dateTo"].get<std::string>();
        QDate dateTo = QDate::fromString(QString::fromStdString(dateToStr), "yyyy-MM-dd");
        booking.setDateTo(dateTo);
    } else if (j.contains("date_to") && !j["date_to"].is_null()) {
        std::string dateToStr = j["date_to"].get<std::string>();
        QDate dateTo = QDate::fromString(QString::fromStdString(dateToStr), "yyyy-MM-dd");
        booking.setDateTo(dateTo);
    } else if (booking.getDateFrom().isValid()) {
        // Default to same day booking if dateTo is missing but dateFrom is valid
        booking.setDateTo(booking.getDateFrom());
    }
    
    return booking;
}

json Booking::toJson() const {
    return {
        {"id", _id},
        {"deskId", _deskId},
        {"userId", _userId},
        {"dateFrom", _dateFrom.toString("yyyy-MM-dd").toStdString()},
        {"dateTo", _dateTo.toString("yyyy-MM-dd").toStdString()}
    };
}

// Desk implementation
Desk::Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber)
    : _id(id), _deskId(deskId), _buildingId(buildingId), _floorNumber(floorNumber) {
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
    Booking booking(bookingId, _id, 0, dateFrom, dateTo);
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