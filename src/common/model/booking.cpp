#include "booking.h"

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
