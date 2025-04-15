#include "booking_repository.h"
#include "../util/logger.h"

BookingRepository::BookingRepository(std::shared_ptr<SQLite::Database> db)
    : _db(db) {
}

std::vector<Booking> BookingRepository::findAll() {
    std::vector<Booking> bookings;

    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings ORDER BY date");

        while (query.executeStep()) {
            Booking booking(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            bookings.push_back(booking);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting all bookings: {}", e.what());
    }

    return bookings;
}

std::optional<Booking> BookingRepository::findById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Booking(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting booking by ID: {}", e.what());
    }

    return std::nullopt;
}

Booking BookingRepository::add(const Booking &booking) {
    try {
        SQLite::Statement query(*_db, "INSERT INTO bookings (desk_id, user_id, date, date_to) "
                                "VALUES (?, ?, ?, ?)");
        query.bind(1, booking.getDeskId());
        query.bind(2, booking.getUserId());
        query.bind(3, booking.getDateFromString());
        query.bind(4, booking.getDateToString());

        query.exec();
        int id = static_cast<int>(_db->getLastInsertRowid());

        Booking newBooking = booking;
        newBooking.setId(id);
        return newBooking;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error adding booking: {}", e.what());
        return booking; // Return original booking with ID 0 to indicate failure
    }
}

bool BookingRepository::update(const Booking &booking) {
    try {
        SQLite::Statement query(*_db, "UPDATE bookings SET desk_id = ?, user_id = ?, date = ?, date_to = ? "
                                "WHERE id = ?");
        query.bind(1, booking.getDeskId());
        query.bind(2, booking.getUserId());
        query.bind(3, booking.getDateFromString());
        query.bind(4, booking.getDateToString());
        query.bind(5, booking.getId());

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error updating booking: {}", e.what());
        return false;
    }
}

bool BookingRepository::remove(int id) {
    try {
        SQLite::Statement query(*_db, "DELETE FROM bookings WHERE id = ?");
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error removing booking: {}", e.what());
        return false;
    }
}

std::vector<Booking> BookingRepository::findByDeskId(int deskId) {
    std::vector<Booking> bookings;

    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE desk_id = ? ORDER BY date");
        query.bind(1, deskId);

        while (query.executeStep()) {
            Booking booking(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            bookings.push_back(booking);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting bookings by desk ID: {}", e.what());
    }

    return bookings;
}

std::vector<Booking> BookingRepository::findByUserId(int userId) {
    std::vector<Booking> bookings;

    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE user_id = ? ORDER BY date");
        query.bind(1, userId);

        while (query.executeStep()) {
            Booking booking(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            bookings.push_back(booking);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting bookings by user ID: {}", e.what());
    }

    return bookings;
}

std::vector<Booking> BookingRepository::findByDate(const std::string &date) {
    std::vector<Booking> bookings;

    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE ? BETWEEN date AND date_to ORDER BY date");
        query.bind(1, date);

        while (query.executeStep()) {
            Booking booking(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            bookings.push_back(booking);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting bookings by date: {}", e.what());
    }

    return bookings;
}

std::vector<Booking> BookingRepository::findByDateRange(int deskId, const std::string &dateFrom,
                                                        const std::string &dateTo) {
    std::vector<Booking> bookings;

    try {
        // Find bookings that overlap with the given range
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE desk_id = ? AND NOT (date_to < ? OR date > ?) "
                                "ORDER BY date");
        query.bind(1, deskId);
        query.bind(2, dateFrom);
        query.bind(3, dateTo);

        while (query.executeStep()) {
            Booking booking(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            bookings.push_back(booking);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting bookings in range: {}", e.what());
    }

    return bookings;
}

bool BookingRepository::hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    try {
        // Check for overlapping bookings, allowing consecutive bookings
        SQLite::Statement query(*_db, "SELECT COUNT(*) FROM bookings "
                                "WHERE desk_id = ? "
                                "AND NOT (date_to < ? OR date > ?) "
                                "AND NOT (date_to = ? OR date = ?)");
        query.bind(1, deskId);
        query.bind(2, dateFrom);
        query.bind(3, dateTo);
        query.bind(4, dateFrom);
        query.bind(5, dateTo);

        if (query.executeStep()) {
            return query.getColumn(0).getInt() > 0;
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error checking for overlapping bookings: {}", e.what());
    }

    return true; // Assume there's an overlap if query fails
}
