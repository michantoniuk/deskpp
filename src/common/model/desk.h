#ifndef DESK_H
#define DESK_H

#include "entity.h"
#include "booking.h"
#include <vector>

class Desk : public Entity {
public:
    Desk() = default;

    Desk(int id, const std::string &name, int buildingId, int floor = 1);

    json toJson() const override;

    std::string toString() const override;

    // Booking-related methods
    bool isAvailable() const { return _bookings.empty(); }

    bool isAvailableOn(const QDate &date) const;

    bool isAvailableForPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    bool hasOverlappingBooking(const QDate &dateFrom, const QDate &dateTo) const;

    const std::vector<Booking> &getBookings() const { return _bookings; }

    Booking getBookingForDate(const QDate &date) const;

    std::vector<Booking> getBookingsAfterDate(const QDate &date) const;

    std::vector<Booking> getBookingsContainingDate(const QDate &date) const;

    void addBooking(const QDate &dateFrom, const QDate &dateTo, int bookingId = 0);

    void addBooking(const Booking &booking);

    void cancelBooking(int bookingId);

    void cancelAllBookings() { _bookings.clear(); }

    // Getters/setters
    const std::string &getName() const { return _name; }
    int getBuildingId() const { return _buildingId; }
    int getFloor() const { return _floor; }
    void setName(const std::string &name) { _name = name; }
    void setBuildingId(int buildingId) { _buildingId = buildingId; }
    void setFloor(int floor) { _floor = floor; }

    // Legacy compatibility
    bool isBooked() const { return !isAvailable(); }
    bool isBookedOnDate(const QDate &date) const { return !isAvailableOn(date); }

private:
    std::string _name;
    int _buildingId;
    int _floor;
    std::vector<Booking> _bookings;

    void sortBookings();
};
#endif
