#include "booking_repository.h"
#include "../util/logger.h"

BookingRepository::BookingRepository(Database& db) : _db(db) {
}

std::vector<Booking> BookingRepository::getBookingsForDate(int deskId, const std::string& date) {
    std::vector<Booking> bookings;

    std::string sql = "SELECT * FROM bookings WHERE desk_id = :desk_id AND :date BETWEEN date AND date_to";
    std::map<std::string, std::string> params = {
        {":desk_id", std::to_string(deskId)},
        {":date", date}
    };

    _db.query(sql, params, [this, &bookings](auto row) {
        bookings.push_back(this->mapRowToBooking(row));
    });

    return bookings;
}

std::vector<Booking> BookingRepository::getBookingsInRange(int deskId,
                                                          const std::string& dateFrom,
                                                          const std::string& dateTo) {
    std::vector<Booking> bookings;

    // Find bookings that overlap with the given range
    std::string sql = "SELECT * FROM bookings WHERE desk_id = " + std::to_string(deskId) +
                     " AND NOT (date_to < '" + dateFrom + "' OR date > '" + dateTo + "')";

    _db.query(sql, [this, &bookings](auto row) {
        bookings.push_back(this->mapRowToBooking(row));
    });

    return bookings;
}

std::vector<Booking> BookingRepository::getActiveBookingsForDesk(int deskId) {
    std::vector<Booking> bookings;

    std::string sql = "SELECT * FROM bookings WHERE desk_id = " + std::to_string(deskId) +
                     " AND date_to >= date('now') ORDER BY date ASC";

    _db.query(sql, [this, &bookings](auto row) {
        bookings.push_back(this->mapRowToBooking(row));
    });

    return bookings;
}

std::optional<Booking> BookingRepository::getBookingById(int bookingId) {
    std::optional<Booking> booking;

    std::string sql = "SELECT * FROM bookings WHERE id = " + std::to_string(bookingId);

    _db.query(sql, [this, &booking](auto row) {
        if (!booking) {
            booking = this->mapRowToBooking(row);
        }
    });

    return booking;
}

bool BookingRepository::hasOverlappingBooking(int deskId,
                                             const std::string& dateFrom,
                                             const std::string& dateTo) {
    bool hasOverlap = false;

    // Check for overlapping bookings, allowing consecutive bookings
    std::string sql = "SELECT COUNT(*) as count FROM bookings WHERE desk_id = " + std::to_string(deskId) +
                     " AND NOT (date_to < '" + dateFrom + "' OR date > '" + dateTo + "')" +
                     " AND NOT (date_to = '" + dateFrom + "' OR date = '" + dateTo + "')";

    _db.query(sql, [&hasOverlap](auto row) {
        hasOverlap = (std::stoi(row["count"]) > 0);
    });

    return hasOverlap;
}

bool BookingRepository::isBookingActive(int bookingId) {
    bool isActive = false;

    std::string sql = "SELECT COUNT(*) as count FROM bookings WHERE id = " + std::to_string(bookingId) +
                     " AND date_to >= date('now')";

    _db.query(sql, [&isActive](auto row) {
        isActive = (std::stoi(row["count"]) > 0);
    });

    return isActive;
}

std::optional<int64_t> BookingRepository::addBooking(int deskId,
                                                   int userId,
                                                   const std::string& dateFrom,
                                                   const std::string& dateTo) {
    std::string sql = "INSERT INTO bookings (desk_id, user_id, date, date_to) VALUES (" +
                     std::to_string(deskId) + ", " +
                     std::to_string(userId) + ", '" +
                     dateFrom + "', '" +
                     dateTo + "')";

    if (!_db.execute(sql)) {
        LOG_ERROR("Failed to add booking for desk {} (user {})", deskId, userId);
        return std::nullopt;
    }

    return _db.lastInsertId();
}

bool BookingRepository::cancelBooking(int bookingId) {
    std::string sql = "DELETE FROM bookings WHERE id = " + std::to_string(bookingId);

    if (!_db.execute(sql)) {
        LOG_ERROR("Failed to cancel booking {}", bookingId);
        return false;
    }

    return true;
}

Booking BookingRepository::mapRowToBooking(const std::map<std::string, std::string>& row) {
    int id = std::stoi(row.at("id"));
    int deskId = std::stoi(row.at("desk_id"));
    int userId = std::stoi(row.at("user_id"));

    const std::string& dateFrom = row.at("date");
    const std::string& dateTo = row.at("date_to");
    
    return Booking(id, deskId, userId, dateFrom, dateTo);
}