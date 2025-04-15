#ifndef BOOKING_REPOSITORY_H
#define BOOKING_REPOSITORY_H

#include "common/repository.h"
#include "common/models.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>

class BookingRepository : public Repository<Booking> {
public:
    explicit BookingRepository(std::shared_ptr<SQLite::Database> db);

    std::vector<Booking> findAll() override;

    std::optional<Booking> findById(int id) override;

    Booking add(const Booking &booking) override;

    bool update(const Booking &booking) override;

    bool remove(int id) override;

    // Additional specialized methods
    std::vector<Booking> findByDeskId(int deskId);

    std::vector<Booking> findByUserId(int userId);

    std::vector<Booking> findByDate(const std::string &date);

    std::vector<Booking> findByDateRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    bool hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo);

private:
    std::shared_ptr<SQLite::Database> _db;
};

#endif // BOOKING_REPOSITORY_H
