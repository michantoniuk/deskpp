#include "services.h"
#include <regex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "../util/logger.h"

// UserService implementation
UserService::UserService(Database& db) : _db(db) {
}

json UserService::getAllUsers() {
    auto users = _db.getAllUsers();
    
    json usersArray = json::array();
    for (const auto& user : users) {
        usersArray.push_back(user.toJson());
    }
    
    return {{"status", "success"}, {"users", usersArray}};
}

json UserService::getUserById(int id) {
    auto user = _db.getUserById(id);
    
    if (!user) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    return {{"status", "success"}, {"user", user->toJson()}};
}

json UserService::getUserByUsername(const std::string& username) {
    auto user = _db.getUserByUsername(username);
    
    if (!user) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    return {{"status", "success"}, {"user", user->toJson()}};
}

json UserService::registerUser(const std::string& username, 
                             const std::string& password,
                             const std::string& email,
                             const std::string& fullName) {
    // Validate input data
    if (!validateUserData(username, password, email)) {
        return {{"status", "error"}, {"message", "Invalid user data"}};
    }
    
    // Create user object
    User user;
    user.setUsername(username);
    user.setEmail(email);
    user.setFullName(fullName);
    
    // Create user in database
    auto userId = _db.createUser(user, password);
    if (!userId) {
        return {{"status", "error"}, {"message", "Username or email already exists"}};
    }
    
    // Get the created user
    auto createdUser = _db.getUserById(*userId);
    if (!createdUser) {
        return {{"status", "error"}, {"message", "Error retrieving user data"}};
    }
    
    return {
        {"status", "success"},
        {"message", "User registered successfully"},
        {"user", createdUser->toJson()}
    };
}

json UserService::loginUser(const std::string& username, const std::string& password) {
    // Hash the password
    std::string passwordHash = hashPassword(password);
    
    // Validate credentials
    bool isValid = _db.validateCredentials(username, passwordHash);
    if (!isValid) {
        return {{"status", "error"}, {"message", "Invalid username or password"}};
    }
    
    // Get user data
    auto user = _db.getUserByUsername(username);
    if (!user) {
        return {{"status", "error"}, {"message", "Error retrieving user data"}};
    }
    
    return {
        {"status", "success"},
        {"message", "Login successful"},
        {"user", user->toJson()}
    };
}

json UserService::updateUser(int id, const json& userData) {
    // Get existing user
    auto existingUser = _db.getUserById(id);
    if (!existingUser) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    User updatedUser = *existingUser;
    
    // Update user fields if provided
    if (userData.contains("username") && !userData["username"].is_null()) {
        updatedUser.setUsername(userData["username"].get<std::string>());
    }
    
    if (userData.contains("password") && !userData["password"].is_null()) {
        std::string passwordHash = hashPassword(userData["password"].get<std::string>());
        updatedUser.setPasswordHash(passwordHash);
    }
    
    if (userData.contains("email") && !userData["email"].is_null()) {
        updatedUser.setEmail(userData["email"].get<std::string>());
    }
    
    if (userData.contains("fullName") && !userData["fullName"].is_null()) {
        updatedUser.setFullName(userData["fullName"].get<std::string>());
    }
    
    // Update user in database
    bool success = _db.updateUser(updatedUser);
    if (!success) {
        return {{"status", "error"}, {"message", "Error updating user"}};
    }
    
    return {
        {"status", "success"},
        {"message", "User updated successfully"},
        {"user", updatedUser.toJson()}
    };
}

json UserService::deleteUser(int id) {
    // Check if user exists
    auto user = _db.getUserById(id);
    if (!user) {
        return {{"status", "error"}, {"message", "User not found"}};
    }
    
    // Delete user
    bool success = _db.deleteUser(id);
    if (!success) {
        return {{"status", "error"}, {"message", "Error deleting user"}};
    }
    
    return {
        {"status", "success"},
        {"message", "User deleted successfully"}
    };
}

bool UserService::validateUserData(const std::string& username, 
                                 const std::string& password,
                                 const std::string& email) {
    // Check if username is valid (alphanumeric, 3-20 chars)
    std::regex usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    if (!std::regex_match(username, usernameRegex)) {
        return false;
    }
    
    // Check if password is valid (at least 6 chars)
    if (password.length() < 6) {
        return false;
    }
    
    // Check if email is valid
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex)) {
        return false;
    }
    
    return true;
}

std::string UserService::hashPassword(const std::string& password) {
    // Simple hashing for demonstration purposes
    // In a real app, use a proper hashing library with salt
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    return std::to_string(hash);
}

// BookingService implementation
BookingService::BookingService(Database& db) : _db(db) {
}

json BookingService::getAllBuildings() {
    auto buildings = _db.getAllBuildings();
    
    json buildingsArray = json::array();
    for (const auto& building : buildings) {
        buildingsArray.push_back(building.toJson());
    }
    
    return {{"status", "success"}, {"buildings", buildingsArray}};
}

json BookingService::getDesksByBuilding(int buildingId) {
    std::vector<Desk> desks;
    
    if (buildingId > 0) {
        desks = _db.getDesksByBuilding(buildingId);
    } else {
        desks = _db.getAllDesks();
    }
    
    json desksArray = json::array();
    for (const auto& desk : desks) {
        json deskJson = desk.toJson();
        
        // Get active bookings for this desk
        auto bookings = _db.getBookingsForDesk(desk.getId());
        
        bool hasBookings = !bookings.empty();
        deskJson["booked"] = hasBookings;
        deskJson["available"] = !hasBookings;
        
        // Add bookings to the JSON response
        json bookingsArray = json::array();
        for (const auto& booking : bookings) {
            bookingsArray.push_back(booking.toJson());
        }
        
        if (hasBookings) {
            deskJson["bookings"] = bookingsArray;
            
            // For backward compatibility, add first booking's dates
            if (!bookings.empty()) {
                deskJson["bookingDateFrom"] = bookings[0].getDateFrom();
                deskJson["bookingDateTo"] = bookings[0].getDateTo();
            }
        }
        
        desksArray.push_back(deskJson);
    }
    
    return {{"status", "success"}, {"desks", desksArray}};
}

json BookingService::getBookingsForDesk(int deskId, const std::string& date) {
    auto bookings = _db.getBookingsForDate(deskId, date);
    
    json bookingsArray = json::array();
    for (const auto& booking : bookings) {
        bookingsArray.push_back(booking.toJson());
    }
    
    return {{"status", "success"}, {"bookings", bookingsArray}};
}

json BookingService::getBookingsForDeskInRange(int deskId, const std::string& dateFrom, const std::string& dateTo) {
    auto bookings = _db.getBookingsInRange(deskId, dateFrom, dateTo);
    
    json bookingsArray = json::array();
    for (const auto& booking : bookings) {
        bookingsArray.push_back(booking.toJson());
    }
    
    return {{"status", "success"}, {"bookings", bookingsArray}};
}

json BookingService::addBooking(int deskId, int userId, const std::string& dateFrom, const std::string& dateTo) {
    // Validate input data
    if (!validateBookingDates(dateFrom, dateTo)) {
        return {{"status", "error"}, {"message", "Invalid date range"}};
    }
    
    // Check if desk exists
    auto desk = _db.getDeskById(deskId);
    if (!desk) {
        return {{"status", "error"}, {"message", "Desk does not exist"}};
    }
    
    // Check if booking is allowed
    if (!isBookingAllowed(deskId, dateFrom, dateTo)) {
        return {{"status", "error"}, {"message", "Desk is already booked in the selected period"}};
    }
    
    // Create booking
    Booking booking;
    booking.setDeskId(deskId);
    booking.setUserId(userId);
    booking.setDateFrom(dateFrom);
    booking.setDateTo(dateTo);
    
    // Add booking
    auto bookingId = _db.addBooking(booking);
    if (!bookingId) {
        return {{"status", "error"}, {"message", "Error adding booking"}};
    }
    
    // Set the booking ID
    booking.setId(*bookingId);
    
    return {
        {"status", "success"},
        {"booking", booking.toJson()}
    };
}

json BookingService::cancelBooking(int bookingId) {
    // Check if booking exists
    auto booking = _db.getBookingById(bookingId);
    if (!booking) {
        return {{"status", "error"}, {"message", "Booking does not exist"}};
    }
    
    // Cancel booking
    bool success = _db.cancelBooking(bookingId);
    if (!success) {
        return {{"status", "error"}, {"message", "Error cancelling booking"}};
    }
    
    return {{"status", "success"}, {"message", "Booking has been canceled"}};
}

bool BookingService::validateBookingDates(const std::string& dateFrom, const std::string& dateTo) {
    // Check if dates are in valid format (YYYY-MM-DD)
    std::regex dateRegex("^\\d{4}-\\d{2}-\\d{2}$");
    if (!std::regex_match(dateFrom, dateRegex) || !std::regex_match(dateTo, dateRegex)) {
        return false;
    }
    
    // Check if date range is valid (from <= to)
    if (dateFrom > dateTo) {
        return false;
    }
    
    // Get current date
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d");
    std::string today = oss.str();
    
    // Check if dates are not in the past
    if (dateFrom < today) {
        return false;
    }
    
    return true;
}

bool BookingService::isBookingAllowed(int deskId, const std::string& dateFrom, const std::string& dateTo) {
    // Check for overlapping bookings
    return !_db.hasOverlappingBooking(deskId, dateFrom, dateTo);
}