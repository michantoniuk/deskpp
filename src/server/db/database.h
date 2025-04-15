#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <SQLiteCpp/SQLiteCpp.h>
#include "../model/models.h"
#include "../util/logger.h"

/**
 * Database access layer for the server application using SQLiteCpp
 */
class Database {
public:
    explicit Database(const std::string &dbPath);

    ~Database() = default;

    // Schema management
    bool initializeSchema();

    bool seedDemoData();

    // User operations
    std::vector<User> getAllUsers();

    std::optional<User> getUserById(int id);

    std::optional<User> getUserByUsername(const std::string &username);

    std::optional<User> getUserByEmail(const std::string &email);

    std::optional<int64_t> createUser(const User &user, const std::string &password);

    bool updateUser(const User &user);

    bool deleteUser(int id);

    bool validateCredentials(const std::string &username, const std::string &passwordHash);

    // Building operations
    std::vector<Building> getAllBuildings();

    std::optional<Building> getBuildingById(int id);

    // Desk operations
    std::vector<Desk> getAllDesks();

    std::vector<Desk> getDesksByBuilding(int buildingId);

    std::optional<Desk> getDeskById(int id);

    bool isDeskAvailable(int deskId, const std::string &date);

    // Booking operations
    std::vector<Booking> getBookingsForDesk(int deskId);

    std::vector<Booking> getBookingsForDate(int deskId, const std::string &date);

    std::vector<Booking> getBookingsInRange(int deskId, const std::string &dateFrom, const std::string &dateTo);

    std::optional<Booking> getBookingById(int bookingId);

    bool hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo);

    std::optional<int64_t> addBooking(const Booking &booking);

    bool cancelBooking(int bookingId);

    // Helper methods for checking schema
    bool tableExists(const std::string &tableName);

private:
    std::unique_ptr<SQLite::Database> _db;

    // Helper methods for password management
    std::string hashPassword(const std::string &password);
};
#endif // SERVER_DATABASE_H
