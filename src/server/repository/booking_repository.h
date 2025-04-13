#ifndef BOOKING_REPOSITORY_H
#define BOOKING_REPOSITORY_H

#include <vector>
#include <string>
#include <optional>
#include "../db/database.h"
#include "../model/booking.h"

class BookingRepository {
public:
    explicit BookingRepository(Database &db);

    std::vector<Booking> getBookingsForDate(int deskId, const std::string &date);

    std::vector<Booking> getBookingsInRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    std::vector<Booking> getActiveBookingsForDesk(int deskId);

    std::optional<Booking> getBookingById(int bookingId);

    bool hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo);

    bool isBookingActive(int bookingId);

    std::optional<int64_t> addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    bool cancelBooking(int bookingId);

private:
    Database &_db;

    Booking mapRowToBooking(const std::map<std::string, std::string> &row);
};

#endif // BOOKING_REPOSITORY_H
