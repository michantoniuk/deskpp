#include "booking.h"

Booking::Booking()
    : _id(0), _deskId(0), _userId(0) {
}

Booking::Booking(int id, int deskId, int userId, const QDate& dateFrom, const QDate& dateTo)
    : _id(id), _deskId(deskId), _userId(userId), _dateFrom(dateFrom), _dateTo(dateTo) {
}

bool Booking::containsDate(const QDate& date) const {
    return date >= _dateFrom && date <= _dateTo;
}

bool Booking::overlapsWithPeriod(const QDate& dateFrom, const QDate& dateTo) const {
    // Special cases - consecutive bookings
    if (dateFrom == _dateTo) {
        return false;
    }

    if (dateTo == _dateFrom) {
        return false;
    }

    // General case - check for overlap
    return !(_dateTo < dateFrom || _dateFrom > dateTo);
}

std::string Booking::toJson() const {
    json j;
    j["id"] = _id;
    j["deskId"] = _deskId;
    j["userId"] = _userId;
    j["dateFrom"] = _dateFrom.toString("yyyy-MM-dd").toStdString();
    j["dateTo"] = _dateTo.toString("yyyy-MM-dd").toStdString();

    return j.dump();
}

Booking Booking::fromJson(const std::string& jsonStr) {
    json j;
    try {
        j = json::parse(jsonStr);
    } catch (...) {
        // Return default booking if parsing fails
        return Booking();
    }

    Booking booking;

    // Parse ID fields
    if (j.contains("id") && !j["id"].is_null()) {
        booking.setId(j["id"].get<int>());
    }

    // Parse desk ID with fallback to alternate field name
    if (j.contains("deskId") && !j["deskId"].is_null()) {
        booking.setDeskId(j["deskId"].get<int>());
    } else if (j.contains("desk_id") && !j["desk_id"].is_null()) {
        booking.setDeskId(j["desk_id"].get<int>());
    }

    // Parse user ID with fallback to alternate field name
    if (j.contains("userId") && !j["userId"].is_null()) {
        booking.setUserId(j["userId"].get<int>());
    } else if (j.contains("user_id") && !j["user_id"].is_null()) {
        booking.setUserId(j["user_id"].get<int>());
    }

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