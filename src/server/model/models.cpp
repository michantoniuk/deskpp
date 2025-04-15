#include "models.h"

// User implementation
User::User(int id, const std::string& username, const std::string& passwordHash,
           const std::string& email, const std::string& fullName)
    : _id(id), _username(username), _passwordHash(passwordHash),
      _email(email), _fullName(fullName) {
}

json User::toJson() const {
    json j;
    j["id"] = _id;
    j["username"] = _username;
    j["email"] = _email;
    j["fullName"] = _fullName;
    // Don't include password hash in JSON for security
    return j;
}

// Building implementation
Building::Building(int id, const std::string& name, const std::string& address)
    : _id(id), _name(name), _address(address) {
}

json Building::toJson() const {
    json j;
    j["id"] = _id;
    j["name"] = _name;
    j["address"] = _address;
    return j;
}

// Desk implementation
Desk::Desk(int id, const std::string& deskId, int buildingId, int floorNumber)
    : _id(id), _deskId(deskId), _buildingId(buildingId), _floorNumber(floorNumber) {
}

json Desk::toJson() const {
    json j;
    j["id"] = _id;
    j["deskId"] = _deskId;
    j["buildingId"] = _buildingId;
    j["floorNumber"] = _floorNumber;
    return j;
}

// Booking implementation
Booking::Booking(int id, int deskId, int userId, const std::string& dateFrom, const std::string& dateTo)
    : _id(id), _deskId(deskId), _userId(userId), _dateFrom(dateFrom), _dateTo(dateTo) {
}

bool Booking::containsDate(const std::string& date) const {
    return !isDateBefore(date, _dateFrom) && !isDateAfter(date, _dateTo);
}

bool Booking::overlapsWithPeriod(const std::string& dateFrom, const std::string& dateTo) const {
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

Booking Booking::fromJson(const json& j) {
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

bool Booking::isDateBefore(const std::string& date1, const std::string& date2) const {
    return date1 < date2;
}

bool Booking::isDateAfter(const std::string& date1, const std::string& date2) const {
    return date1 > date2;
}

bool Booking::isDateEqual(const std::string& date1, const std::string& date2) const {
    return date1 == date2;
}