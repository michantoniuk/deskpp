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
    return containsDate(QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd"));
}

bool Booking::overlapsWithPeriod(const QDate &dateFrom, const QDate &dateTo) const {
    if (dateFrom == _dateTo.addDays(1) || dateTo.addDays(1) == _dateFrom) {
        return false;
    }
    return !(dateTo < _dateFrom || dateFrom > _dateTo);
}

bool Booking::overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const {
    return overlapsWithPeriod(
        QDate::fromString(QString::fromStdString(dateFrom), "yyyy-MM-dd"),
        QDate::fromString(QString::fromStdString(dateTo), "yyyy-MM-dd")
    );
}

Booking Booking::fromJson(const json &j) {
    Booking booking;
    if (j.contains("id") && !j["id"].is_null())
        booking.setId(j["id"].get<int>());

    if (j.contains("deskId") && !j["deskId"].is_null())
        booking.setDeskId(j["deskId"].get<int>());
    else if (j.contains("desk_id") && !j["desk_id"].is_null())
        booking.setDeskId(j["desk_id"].get<int>());

    if (j.contains("userId") && !j["userId"].is_null())
        booking.setUserId(j["userId"].get<int>());
    else if (j.contains("user_id") && !j["user_id"].is_null())
        booking.setUserId(j["user_id"].get<int>());

    if (j.contains("dateFrom") && !j["dateFrom"].is_null())
        booking.setDateFrom(j["dateFrom"].get<std::string>());
    else if (j.contains("date") && !j["date"].is_null())
        booking.setDateFrom(j["date"].get<std::string>());

    if (j.contains("dateTo") && !j["dateTo"].is_null())
        booking.setDateTo(j["dateTo"].get<std::string>());
    else if (j.contains("date_to") && !j["date_to"].is_null())
        booking.setDateTo(j["date_to"].get<std::string>());
    else if (booking.getDateFrom().isValid())
        booking.setDateTo(booking.getDateFrom());

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

void Booking::setDateFrom(const std::string &dateFrom) {
    _dateFrom = QDate::fromString(QString::fromStdString(dateFrom), "yyyy-MM-dd");
}

void Booking::setDateTo(const std::string &dateTo) {
    _dateTo = QDate::fromString(QString::fromStdString(dateTo), "yyyy-MM-dd");
}
