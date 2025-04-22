#include "booking_repository.h"

BookingRepository::BookingRepository(std::shared_ptr<SQLite::Database> db)
    : SQLiteRepository<Booking>(
        db,
        "bookings",
        "SELECT id, desk_id, user_id, date, date_to FROM bookings ORDER BY date",
        "SELECT id, desk_id, user_id, date, date_to FROM bookings WHERE id = ?",
        "INSERT INTO bookings (desk_id, user_id, date, date_to) VALUES (?, ?, ?, ?)",
        "UPDATE bookings SET desk_id = ?, user_id = ?, date = ?, date_to = ? WHERE id = ?",
        "DELETE FROM bookings WHERE id = ?",
        bookingFromRow,
        [](SQLite::Statement &stmt, const Booking &booking) {
            stmt.bind(1, booking.getDeskId());
            stmt.bind(2, booking.getUserId());
            stmt.bind(3, booking.getDateFromString());
            stmt.bind(4, booking.getDateToString());
        }
    ) {
}

Booking BookingRepository::bookingFromRow(SQLite::Statement &query) {
    return Booking(
        query.getColumn(0).getInt(),
        query.getColumn(1).getInt(),
        query.getColumn(2).getInt(),
        query.getColumn(3).getString(),
        query.getColumn(4).getString()
    );
}

std::vector<Booking> BookingRepository::findByDeskId(int deskId) {
    std::vector<Booking> bookings;
    SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                            "FROM bookings WHERE desk_id = ? ORDER BY date");
    query.bind(1, deskId);

    while (query.executeStep()) {
        bookings.push_back(bookingFromRow(query));
    }
    return bookings;
}

std::vector<Booking> BookingRepository::findByUserId(int userId) {
    std::vector<Booking> bookings;
    SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                            "FROM bookings WHERE user_id = ? ORDER BY date");
    query.bind(1, userId);

    while (query.executeStep()) {
        bookings.push_back(bookingFromRow(query));
    }
    return bookings;
}

std::vector<Booking> BookingRepository::findByDateRange(int deskId, const std::string &dateFrom,
                                                        const std::string &dateTo) {
    std::vector<Booking> bookings;
    SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                            "FROM bookings WHERE desk_id = ? AND NOT (date_to < ? OR date > ?) "
                            "ORDER BY date");
    query.bind(1, deskId);
    query.bind(2, dateFrom);
    query.bind(3, dateTo);

    while (query.executeStep()) {
        bookings.push_back(bookingFromRow(query));
    }
    return bookings;
}

bool BookingRepository::hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    SQLite::Statement query(*_db, "SELECT COUNT(*) FROM bookings "
                            "WHERE desk_id = ? AND NOT (date_to < ? OR date > ?)");
    query.bind(1, deskId);
    query.bind(2, dateFrom);
    query.bind(3, dateTo);

    if (query.executeStep()) {
        return query.getColumn(0).getInt() > 0;
    }
    return false;
}
