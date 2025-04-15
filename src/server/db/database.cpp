#include "database.h"
#include <functional>
#include <chrono>

Database::Database(const std::string &dbPath) {
    try {
        _db = std::make_unique<SQLite::Database>(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        LOG_INFO("Connected to database: {}", dbPath);
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("SQLite exception: {}", e.what());
        throw;
    }
}

bool Database::initializeSchema() {
    try {
        _db->exec("BEGIN TRANSACTION;");

        // Buildings table
        if (!tableExists("buildings")) {
            _db->exec("CREATE TABLE buildings ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "address TEXT"
                ");");
        }

        // Desks table
        if (!tableExists("desks")) {
            _db->exec("CREATE TABLE desks ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "building_id INTEGER NOT NULL,"
                "floor_number INTEGER NOT NULL,"
                "FOREIGN KEY (building_id) REFERENCES buildings(id) ON DELETE CASCADE"
                ");");
        }

        // Users table
        if (!tableExists("users")) {
            _db->exec("CREATE TABLE users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "username TEXT NOT NULL UNIQUE,"
                "password_hash TEXT NOT NULL,"
                "email TEXT NOT NULL UNIQUE,"
                "full_name TEXT,"
                "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                ");");
        }

        // Bookings table
        if (!tableExists("bookings")) {
            _db->exec("CREATE TABLE bookings ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "desk_id INTEGER NOT NULL,"
                "user_id INTEGER NOT NULL,"
                "date TEXT NOT NULL,"
                "date_to TEXT NOT NULL,"
                "FOREIGN KEY (desk_id) REFERENCES desks(id) ON DELETE CASCADE,"
                "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
                ");");
        }

        _db->exec("COMMIT;");
        LOG_INFO("Database schema initialization completed successfully");
        return true;
    } catch (const SQLite::Exception &e) {
        _db->exec("ROLLBACK;");
        LOG_ERROR("Database schema initialization failed: {}", e.what());
        return false;
    }
}

bool Database::seedDemoData() {
    try {
        _db->exec("BEGIN TRANSACTION;");

        // Check if buildings table has data
        SQLite::Statement queryBuildings(*_db, "SELECT COUNT(*) FROM buildings");
        if (queryBuildings.executeStep() && queryBuildings.getColumn(0).getInt() == 0) {
            // Add sample buildings
            _db->exec("INSERT INTO buildings (name, address) VALUES "
                "('Krakow A', 'Krakowska St. 123'),"
                "('Warsaw B', 'Warszawska St. 456');");
            LOG_INFO("Added sample buildings to database");
        }

        // Check if desks table has data
        SQLite::Statement queryDesks(*_db, "SELECT COUNT(*) FROM desks");
        if (queryDesks.executeStep() && queryDesks.getColumn(0).getInt() == 0) {
            // Get building IDs
            int krakAId = 0, wawBId = 0;

            SQLite::Statement queryKrakA(*_db, "SELECT id FROM buildings WHERE name = 'Krakow A'");
            if (queryKrakA.executeStep()) {
                krakAId = queryKrakA.getColumn(0).getInt();
            }

            SQLite::Statement queryWawB(*_db, "SELECT id FROM buildings WHERE name = 'Warsaw B'");
            if (queryWawB.executeStep()) {
                wawBId = queryWawB.getColumn(0).getInt();
            }

            // Add sample desks
            if (krakAId > 0 && wawBId > 0) {
                _db->exec("INSERT INTO desks (name, building_id, floor_number) VALUES "
                          "('KrakA-01-001', " + std::to_string(krakAId) + ", 1),"
                          "('KrakA-01-002', " + std::to_string(krakAId) + ", 1),"
                          "('KrakA-01-003', " + std::to_string(krakAId) + ", 1),"
                          "('WawB-01-001', " + std::to_string(wawBId) + ", 1),"
                          "('WawB-01-002', " + std::to_string(wawBId) + ", 1);");
                LOG_INFO("Added sample desks to database");
            }
        }

        // Check if users table has data
        SQLite::Statement queryUsers(*_db, "SELECT COUNT(*) FROM users");
        if (queryUsers.executeStep() && queryUsers.getColumn(0).getInt() == 0) {
            // Add sample users with hashed passwords (password is "password")
            std::string passwordHash = hashPassword("password");

            _db->exec("INSERT INTO users (username, password_hash, email, full_name) VALUES "
                      "('admin', '" + passwordHash + "', 'admin@example.com', 'Admin User'),"
                      "('user1', '" + passwordHash + "', 'user1@example.com', 'Regular User'),"
                      "('user2', '" + passwordHash + "', 'user2@example.com', 'Another User');");
            LOG_INFO("Added sample users to database");
        }

        _db->exec("COMMIT;");
        LOG_INFO("Successfully added sample data to database");
        return true;
    } catch (const SQLite::Exception &e) {
        _db->exec("ROLLBACK;");
        LOG_ERROR("Error adding sample data: {}", e.what());
        return false;
    }
}

std::vector<User> Database::getAllUsers() {
    std::vector<User> users;

    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users ORDER BY username");

        while (query.executeStep()) {
            User user(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
            users.push_back(user);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting users: {}", e.what());
    }

    return users;
}

std::optional<User> Database::getUserById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return User(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting user by ID: {}", e.what());
    }

    return std::nullopt;
}

std::optional<User> Database::getUserByUsername(const std::string &username) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE username = ?");
        query.bind(1, username);

        if (query.executeStep()) {
            return User(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting user by username: {}", e.what());
    }

    return std::nullopt;
}

std::optional<User> Database::getUserByEmail(const std::string &email) {
    try {
        SQLite::Statement query(*_db, "SELECT id, username, password_hash, email, full_name "
                                "FROM users WHERE email = ?");
        query.bind(1, email);

        if (query.executeStep()) {
            return User(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting user by email: {}", e.what());
    }

    return std::nullopt;
}

std::optional<int64_t> Database::createUser(const User &user, const std::string &password) {
    // Check if username or email already exists
    if (getUserByUsername(user.getUsername()) || getUserByEmail(user.getEmail())) {
        return std::nullopt;
    }

    try {
        SQLite::Statement query(*_db, "INSERT INTO users (username, password_hash, email, full_name) "
                                "VALUES (?, ?, ?, ?)");
        query.bind(1, user.getUsername());
        query.bind(2, hashPassword(password));
        query.bind(3, user.getEmail());
        query.bind(4, user.getFullName());

        query.exec();
        return _db->getLastInsertRowid();
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error creating user: {}", e.what());
        return std::nullopt;
    }
}

bool Database::updateUser(const User &user) {
    try {
        SQLite::Statement query(*_db, "UPDATE users SET username = ?, password_hash = ?, "
                                "email = ?, full_name = ? WHERE id = ?");
        query.bind(1, user.getUsername());
        query.bind(2, user.getPasswordHash());
        query.bind(3, user.getEmail());
        query.bind(4, user.getFullName());
        query.bind(5, user.getId());

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error updating user: {}", e.what());
        return false;
    }
}

bool Database::deleteUser(int id) {
    try {
        SQLite::Statement query(*_db, "DELETE FROM users WHERE id = ?");
        query.bind(1, id);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error deleting user: {}", e.what());
        return false;
    }
}

bool Database::validateCredentials(const std::string &username, const std::string &passwordHash) {
    try {
        SQLite::Statement query(*_db, "SELECT COUNT(*) FROM users "
                                "WHERE username = ? AND password_hash = ?");
        query.bind(1, username);
        query.bind(2, passwordHash);

        if (query.executeStep()) {
            return query.getColumn(0).getInt() > 0;
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error validating credentials: {}", e.what());
    }

    return false;
}

std::vector<Building> Database::getAllBuildings() {
    std::vector<Building> buildings;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, address FROM buildings ORDER BY name");

        while (query.executeStep()) {
            Building building(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString()
            );
            buildings.push_back(building);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting buildings: {}", e.what());
    }

    return buildings;
}

std::optional<Building> Database::getBuildingById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, address FROM buildings WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Building(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting building by ID: {}", e.what());
    }

    return std::nullopt;
}

std::vector<Desk> Database::getAllDesks() {
    std::vector<Desk> desks;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number FROM desks");

        while (query.executeStep()) {
            Desk desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
            desks.push_back(desk);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting all desks: {}", e.what());
    }

    return desks;
}

std::vector<Desk> Database::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;

    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE building_id = ?");
        query.bind(1, buildingId);

        while (query.executeStep()) {
            Desk desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
            desks.push_back(desk);
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desks by building: {}", e.what());
    }

    return desks;
}

std::optional<Desk> Database::getDeskById(int id) {
    try {
        SQLite::Statement query(*_db, "SELECT id, name, building_id, floor_number "
                                "FROM desks WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Desk(
                query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getInt()
            );
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error getting desk by ID: {}", e.what());
    }

    return std::nullopt;
}

bool Database::isDeskAvailable(int deskId, const std::string &date) {
    try {
        SQLite::Statement query(*_db, "SELECT COUNT(*) FROM bookings "
                                "WHERE desk_id = ? AND ? BETWEEN date AND date_to");
        query.bind(1, deskId);
        query.bind(2, date);

        if (query.executeStep()) {
            return query.getColumn(0).getInt() == 0;
        }
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error checking desk availability: {}", e.what());
    }

    return false;
}

std::vector<Booking> Database::getBookingsForDesk(int deskId) {
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
        LOG_ERROR("Error getting bookings for desk: {}", e.what());
    }

    return bookings;
}

std::vector<Booking> Database::getBookingsForDate(int deskId, const std::string &date) {
    std::vector<Booking> bookings;

    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE desk_id = ? AND ? BETWEEN date AND date_to");
        query.bind(1, deskId);
        query.bind(2, date);

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
        LOG_ERROR("Error getting bookings for date: {}", e.what());
    }

    return bookings;
}

std::vector<Booking> Database::getBookingsInRange(int deskId, const std::string &dateFrom, const std::string &dateTo) {
    std::vector<Booking> bookings;

    try {
        // Find bookings that overlap with the given range
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE desk_id = ? AND NOT (date_to < ? OR date > ?)");
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

std::optional<Booking> Database::getBookingById(int bookingId) {
    try {
        SQLite::Statement query(*_db, "SELECT id, desk_id, user_id, date, date_to "
                                "FROM bookings WHERE id = ?");
        query.bind(1, bookingId);

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

bool Database::hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo) {
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

std::optional<int64_t> Database::addBooking(const Booking &booking) {
    try {
        SQLite::Statement query(*_db, "INSERT INTO bookings (desk_id, user_id, date, date_to) "
                                "VALUES (?, ?, ?, ?)");
        query.bind(1, booking.getDeskId());
        query.bind(2, booking.getUserId());
        query.bind(3, booking.getDateFrom());
        query.bind(4, booking.getDateTo());

        query.exec();
        return _db->getLastInsertRowid();
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error adding booking: {}", e.what());
        return std::nullopt;
    }
}

bool Database::cancelBooking(int bookingId) {
    try {
        SQLite::Statement query(*_db, "DELETE FROM bookings WHERE id = ?");
        query.bind(1, bookingId);

        query.exec();
        return query.getChanges() > 0;
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error canceling booking: {}", e.what());
        return false;
    }
}

bool Database::tableExists(const std::string &tableName) {
    try {
        SQLite::Statement query(*_db, "SELECT name FROM sqlite_master WHERE type='table' AND name=?");
        query.bind(1, tableName);

        return query.executeStep();
    } catch (const SQLite::Exception &e) {
        LOG_ERROR("Error checking if table exists: {}", e.what());
        return false;
    }
}

std::string Database::hashPassword(const std::string &password) {
    // Simple hashing for demonstration purposes
    // In a real app, use a proper hashing library with salt
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    return std::to_string(hash);
}
