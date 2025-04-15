#ifndef SERVER_SERVICES_H
#define SERVER_SERVICES_H

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "../db/database.h"
#include "../model/models.h"

using json = nlohmann::json;

/**
 * Service layer for user operations
 */
class UserService {
public:
    explicit UserService(Database& db);
    
    json getAllUsers();
    json getUserById(int id);
    json getUserByUsername(const std::string& username);
    
    json registerUser(const std::string& username, 
                     const std::string& password,
                     const std::string& email,
                     const std::string& fullName);
    
    json loginUser(const std::string& username, const std::string& password);
    json updateUser(int id, const json& userData);
    json deleteUser(int id);
    
private:
    Database& _db;
    
    // Helpers
    bool validateUserData(const std::string& username, 
                          const std::string& password,
                          const std::string& email);
    
    std::string hashPassword(const std::string& password);
};

/**
 * Service layer for booking operations
 */
class BookingService {
public:
    explicit BookingService(Database& db);
    
    json getAllBuildings();
    json getDesksByBuilding(int buildingId = -1);
    json getBookingsForDesk(int deskId, const std::string& date);
    json getBookingsForDeskInRange(int deskId, const std::string& dateFrom, const std::string& dateTo);
    
    json addBooking(int deskId, int userId, const std::string& dateFrom, const std::string& dateTo);
    json cancelBooking(int bookingId);
    
private:
    Database& _db;
    
    // Validation helpers
    bool validateBookingDates(const std::string& dateFrom, const std::string& dateTo);
    bool isBookingAllowed(int deskId, const std::string& dateFrom, const std::string& dateTo);
};

#endif // SERVER_SERVICES_H