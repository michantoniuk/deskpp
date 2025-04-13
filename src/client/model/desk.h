#ifndef CLIENT_MODEL_DESK_H
#define CLIENT_MODEL_DESK_H

#include <string>
#include <QDate>
#include <vector>
#include "booking.h"

class Desk {
public:
    Desk();

    Desk(int id, const std::string &deskId, const std::string &buildingId, int floorNumber);

    // Getters
    int getId() const;

    std::string getDeskId() const;

    std::string getBuildingId() const;

    int getFloorNumber() const;

    // Setters
    void setId(int id);

    void setDeskId(const std::string &deskId);

    void setBuildingId(const std::string &buildingId);

    void setFloorNumber(int floorNumber);

    // Simplified booking related methods
    bool isAvailable() const;

    bool isAvailableOn(const QDate &date) const;

    bool isAvailableForPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    const std::vector<Booking> &getBookings() const;

    Booking getBookingForDate(const QDate &date) const;

    std::vector<Booking> getBookingsAfterDate(const QDate &date) const;

    std::vector<Booking> getBookingsContainingDate(const QDate &date) const;

    void addBooking(const QDate &dateFrom, const QDate &dateTo, int bookingId = 0);

    void addBooking(const Booking &booking);

    void cancelBooking(int bookingId);

    void cancelAllBookings();

    // For backward compatibility (deprecated)
    bool isBooked() const { return !isAvailable(); }
    bool isBookedOnDate(const QDate &date) const { return !isAvailableOn(date); }

    bool hasOverlappingBooking(const QDate &dateFrom, const QDate &dateTo) const;

    bool hasNoBookings() const { return isAvailable(); }

private:
    int _id;
    std::string _deskId;
    std::string _buildingId;
    int _floorNumber;
    std::vector<Booking> _bookings;

    // Helper methods
    void sortBookings();
};

#endif // CLIENT_MODEL_DESK_H
