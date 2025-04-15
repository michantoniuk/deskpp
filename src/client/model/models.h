#ifndef CLIENT_MODEL_MODELS_H
#define CLIENT_MODEL_MODELS_H

#include <QDate>
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * User model for the client application
 */
class User {
public:
    User() = default;
    User(int id, const std::string &username, const std::string &email, const std::string &fullName);

    // JSON serialization
    static User fromJson(const json &j);
    json toJson() const;

    // Getters and setters
    int getId() const { return _id; }
    const std::string& getUsername() const { return _username; }
    const std::string& getEmail() const { return _email; }
    const std::string& getFullName() const { return _fullName; }

    void setId(int id) { _id = id; }
    void setUsername(const std::string &username) { _username = username; }
    void setEmail(const std::string &email) { _email = email; }
    void setFullName(const std::string &fullName) { _fullName = fullName; }

private:
    int _id = 0;
    std::string _username;
    std::string _email;
    std::string _fullName;
};

/**
 * Booking model for the client application
 */
class Booking {
public:
    Booking() = default;
    Booking(int id, int deskId, int userId, const QDate& dateFrom, const QDate& dateTo);

    // Core functionality
    bool containsDate(const QDate& date) const;
    bool overlapsWithPeriod(const QDate& dateFrom, const QDate& dateTo) const;

    // JSON serialization
    static Booking fromJson(const json &j);
    json toJson() const;

    // Getters and setters
    int getId() const { return _id; }
    int getDeskId() const { return _deskId; }
    int getUserId() const { return _userId; }
    QDate getDateFrom() const { return _dateFrom; }
    QDate getDateTo() const { return _dateTo; }

    void setId(int id) { _id = id; }
    void setDeskId(int deskId) { _deskId = deskId; }
    void setUserId(int userId) { _userId = userId; }
    void setDateFrom(const QDate& dateFrom) { _dateFrom = dateFrom; }
    void setDateTo(const QDate& dateTo) { _dateTo = dateTo; }

private:
    int _id = 0;
    int _deskId = 0;
    int _userId = 0;
    QDate _dateFrom;
    QDate _dateTo;
};

/**
 * Desk model for the client application
 */
class Desk {
public:
    Desk() = default;
    Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber);

    // Booking-related methods
    bool isAvailable() const;
    bool isAvailableOn(const QDate &date) const;
    bool isAvailableForPeriod(const QDate &dateFrom, const QDate &dateTo) const;
    bool hasOverlappingBooking(const QDate &dateFrom, const QDate &dateTo) const;

    const std::vector<Booking> &getBookings() const;
    Booking getBookingForDate(const QDate &date) const;
    std::vector<Booking> getBookingsAfterDate(const QDate &date) const;
    std::vector<Booking> getBookingsContainingDate(const QDate &date) const;

    void addBooking(const QDate &dateFrom, const QDate &dateTo, int bookingId = 0);
    void addBooking(const Booking &booking);
    void cancelBooking(int bookingId);
    void cancelAllBookings();

    // Getters and setters
    int getId() const { return _id; }
    const std::string& getDeskId() const { return _deskId; }
    const std::string& getBuildingId() const { return _buildingId; }
    int getFloorNumber() const { return _floorNumber; }

    void setId(int id) { _id = id; }
    void setDeskId(const std::string &deskId) { _deskId = deskId; }
    void setBuildingId(const std::string &buildingId) { _buildingId = buildingId; }
    void setFloorNumber(int floorNumber) { _floorNumber = floorNumber; }

    // Legacy methods for backward compatibility
    bool isBooked() const { return !isAvailable(); }
    bool isBookedOnDate(const QDate &date) const { return !isAvailableOn(date); }
    bool hasNoBookings() const { return isAvailable(); }

private:
    int _id = 0;
    std::string _deskId;
    std::string _buildingId;
    int _floorNumber = 0;
    std::vector<Booking> _bookings;

    void sortBookings();
};

#endif // CLIENT_MODEL_MODELS_H