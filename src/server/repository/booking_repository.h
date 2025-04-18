#ifndef BOOKING_REPOSITORY_H
#define BOOKING_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/booking.h"
#include <memory>

class BookingRepository : public SQLiteRepository<Booking> {
public:
    explicit BookingRepository(std::shared_ptr<SQLite::Database> db);

    std::vector<Booking> findByDeskId(int deskId);

    std::vector<Booking> findByUserId(int userId);

    std::vector<Booking> findByDate(const std::string &date);

    std::vector<Booking> findByDateRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    bool hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo);

private:
    static Booking bookingFromRow(SQLite::Statement &query);
};

#endif // BOOKING_REPOSITORY_H
