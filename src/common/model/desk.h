#ifndef DESK_H
#define DESK_H

#include "entity.h"
#include "booking.h"
#include <string>
#include <vector>
#include <QDate>

// Desk model
class Desk : public Entity {
public:
    Desk() = default;

    Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber,
         int locationX = 0, int locationY = 0);

    // From Entity
    json toJson() const override;

    std::string toString() const override;

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

    // Getters/setters
    const std::string &getDeskId() const { return _deskId; }
    const std::string &getBuildingId() const { return _buildingId; }
    int getFloorNumber() const { return _floorNumber; }
    int getLocationX() const { return _locationX; }
    int getLocationY() const { return _locationY; }

    void setDeskId(const std::string &deskId) { _deskId = deskId; }
    void setBuildingId(const std::string &buildingId) { _buildingId = buildingId; }
    void setFloorNumber(int floorNumber) { _floorNumber = floorNumber; }
    void setLocationX(int x) { _locationX = x; }
    void setLocationY(int y) { _locationY = y; }

    // Legacy methods for backward compatibility
    bool isBooked() const { return !isAvailable(); }
    bool isBookedOnDate(const QDate &date) const { return !isAvailableOn(date); }
    bool hasNoBookings() const { return isAvailable(); }

private:
    std::string _deskId;
    std::string _buildingId;
    int _floorNumber = 0;
    std::vector<Booking> _bookings;
    int _locationX = 0;
    int _locationY = 0;

    void sortBookings();
};

#endif // DESK_H
