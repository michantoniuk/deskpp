#ifndef SERVER_CONTROLLERS_H
#define SERVER_CONTROLLERS_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include "../service/services.h"

using json = nlohmann::json;

/**
 * Controller for booking-related API endpoints
 */
class BookingController {
public:
    explicit BookingController(BookingService& bookingService);
    
    crow::response getBuildings(const crow::request& req);
    crow::response getDesks(const crow::request& req);
    crow::response getBookings(const crow::request& req);
    crow::response addBooking(const crow::request& req);
    crow::response cancelBooking(int bookingId);
    
private:
    BookingService& _bookingService;
    
    // Helper methods
    json parseJson(const std::string& body);
    crow::response errorResponse(int statusCode, const std::string& message);
    crow::response successResponse(const json& data);
    std::optional<json> validateRequest(const crow::request& req,
                                        const std::vector<std::string>& requiredFields);
};

/**
 * Controller for user-related API endpoints
 */
class UserController {
public:
    explicit UserController(UserService& userService);
    
    crow::response getUsers(const crow::request& req);
    crow::response getUser(int id);
    crow::response registerUser(const crow::request& req);
    crow::response loginUser(const crow::request& req);
    crow::response updateUser(int id, const crow::request& req);
    crow::response deleteUser(int id);
    
private:
    UserService& _userService;
    
    // Helper methods
    json parseJson(const std::string& body);
    crow::response errorResponse(int statusCode, const std::string& message);
    crow::response successResponse(const json& data);
    std::optional<json> validateRequest(const crow::request& req,
                                        const std::vector<std::string>& requiredFields);
};

/**
 * Register all API routes with the Crow application
 */
void registerRoutes(crow::SimpleApp& app, BookingController& bookingController, UserController& userController);

#endif // SERVER_CONTROLLERS_H