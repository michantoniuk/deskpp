#include "booking.h"
#include <iostream>

Booking::Booking()
    : _id(0), _deskId(0), _userId(0) {
}

Booking::Booking(int id, int deskId, int userId, const std::string &dateFrom, const std::string &dateTo)
    : _id(id), _deskId(deskId), _userId(userId), _dateFrom(dateFrom), _dateTo(dateTo) {
}

bool Booking::containsDate(const std::string &date) const {
    return !isDateBefore(date, _dateFrom) && !isDateAfter(date, _dateTo);
}

bool Booking::overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const {
    // Two date ranges overlap if one doesn't end before the other starts
    // and one doesn't start after the other ends
    return !(isDateBefore(_dateTo, dateFrom) || isDateAfter(_dateFrom, dateTo));
}

json Booking::toJson() const {
    json j;
    j["id"] = _id;
    j["deskId"] = _deskId;
    j["userId"] = _userId;
    j["dateFrom"] = _dateFrom;
    j["dateTo"] = _dateTo;
    return j;
}

Booking Booking::fromJson(const json &j) {
    Booking booking;

    if (j.contains("id") && !j["id"].is_null()) {
        booking.setId(j["id"].get<int>());
    }

    if (j.contains("deskId") && !j["deskId"].is_null()) {
        booking.setDeskId(j["deskId"].get<int>());
    } else if (j.contains("desk_id") && !j["desk_id"].is_null()) {
        booking.setDeskId(j["desk_id"].get<int>());
    }

    if (j.contains("userId") && !j["userId"].is_null()) {
        booking.setUserId(j["userId"].get<int>());
    } else if (j.contains("user_id") && !j["user_id"].is_null()) {
        booking.setUserId(j["user_id"].get<int>());
    }

    if (j.contains("dateFrom") && !j["dateFrom"].is_null()) {
        booking.setDateFrom(j["dateFrom"].get<std::string>());
    } else if (j.contains("date") && !j["date"].is_null()) {
        booking.setDateFrom(j["date"].get<std::string>());
    }

    if (j.contains("dateTo") && !j["dateTo"].is_null()) {
        booking.setDateTo(j["dateTo"].get<std::string>());
    } else if (j.contains("date_to") && !j["date_to"].is_null()) {
        booking.setDateTo(j["date_to"].get<std::string>());
    } else if (!booking.getDateFrom().empty()) {
        // Default to same day booking if dateTo is missing but dateFrom is valid
        booking.setDateTo(booking.getDateFrom());
    }

    return booking;
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
