#ifndef COMMON_MODELS_H
#define COMMON_MODELS_H

#include "entity.h"
#include <string>
#include <QDate>
#include <vector>

// User model
class User : public Entity {
public:
    User() = default;

    User(int id, const std::string &username, const std::string &email, const std::string &fullName);

    // From Entity
    json toJson() const override;

    std::string toString() const override;

    // Getters/setters
    const std::string &getUsername() const { return _username; }
    const std::string &getEmail() const { return _email; }
    const std::string &getFullName() const { return _fullName; }
    const std::string &getPasswordHash() const { return _passwordHash; }

    void setUsername(const std::string &username) { _username = username; }
    void setEmail(const std::string &email) { _email = email; }
    void setFullName(const std::string &fullName) { _fullName = fullName; }
    void setPasswordHash(const std::string &passwordHash) { _passwordHash = passwordHash; }

private:
    std::string _username;
    std::string _email;
    std::string _fullName;
    std::string _passwordHash;
};

// Building model
class Building : public Entity {
public:
    Building() = default;

    Building(int id, const std::string &name, const std::string &address);

    // From Entity
    json toJson() const override;

    std::string toString() const override;

    // Getters/setters
    const std::string &getName() const { return _name; }
    const std::string &getAddress() const { return _address; }

    void setName(const std::string &name) { _name = name; }
    void setAddress(const std::string &address) { _address = address; }

private:
    std::string _name;
    std::string _address;
};

// Desk model
class Desk : public Entity {
public:
    Desk() = default;

    Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber);

    Desk(int id, const std::string &deskId, int buildingId, int floorNumber);

    // From Entity
    json toJson() const override;

    std::string toString() const override;

    // Booking-related methods
    bool isAvailable() const;

    bool isAvailableOn(const QDate &date) const;

    bool isAvailableForPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    bool hasOverlappingBooking(const QDate &dateFrom, const QDate &dateTo) const;

    const std::vector<class Booking> &getBookings() const;

    Booking getBookingForDate(const QDate &date) const;

    std::vector<Booking> getBookingsAfterDate(const QDate &date) const;

    std::vector<Booking> getBookingsContainingDate(const QDate &date) const;

    void addBooking(const QDate &dateFrom, const QDate &dateTo, int bookingId = 0);

    void addBooking(const Booking &booking);

    void cancelBooking(int bookingId);

    void cancelAllBookings();

    // Getters/setters
    const std::string &getDeskId() const { return _deskId; }
    const std::string &getBuildingId() const { return _buildingId; }
    int getFloorNumber() const { return _floorNumber; }

    void setDeskId(const std::string &deskId) { _deskId = deskId; }
    void setBuildingId(const std::string &buildingId) { _buildingId = buildingId; }
    void setFloorNumber(int floorNumber) { _floorNumber = floorNumber; }

    // Legacy methods for backward compatibility
    bool isBooked() const { return !isAvailable(); }
    bool isBookedOnDate(const QDate &date) const { return !isAvailableOn(date); }
    bool hasNoBookings() const { return isAvailable(); }

private:
    std::string _deskId;
    std::string _buildingId;
    int _floorNumber = 0;
    std::vector<class Booking> _bookings;

    void sortBookings();
};

// Booking model
class Booking : public Entity {
public:
    Booking() = default;

    Booking(int id, int deskId, int userId, const QDate &dateFrom, const QDate &dateTo);

    Booking(int id, int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    // From Entity
    json toJson() const override;

    std::string toString() const override;

    // Core functionality
    bool containsDate(const QDate &date) const;

    bool containsDate(const std::string &date) const;

    bool overlapsWithPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    bool overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const;

    // JSON conversion
    static Booking fromJson(const json &j);

    // Getters/setters
    int getDeskId() const { return _deskId; }
    int getUserId() const { return _userId; }
    QDate getDateFrom() const { return _dateFrom; }
    QDate getDateTo() const { return _dateTo; }

    std::string getDateFromString() const;

    std::string getDateToString() const;

    void setDeskId(int deskId) { _deskId = deskId; }
    void setUserId(int userId) { _userId = userId; }
    void setDateFrom(const QDate &dateFrom) { _dateFrom = dateFrom; }
    void setDateTo(const QDate &dateTo) { _dateTo = dateTo; }

    void setDateFrom(const std::string &dateFrom);

    void setDateTo(const std::string &dateTo);

private:
    int _deskId = 0;
    int _userId = 0;
    QDate _dateFrom;
    QDate _dateTo;

    // Helper methods for date comparison
    bool isDateBefore(const std::string &date1, const std::string &date2) const;

    bool isDateAfter(const std::string &date1, const std::string &date2) const;

    bool isDateEqual(const std::string &date1, const std::string &date2) const;
};

#endif // COMMON_MODELS_H
