#include "routes.h"
#include "common/logger.h"
#include "controller/booking_controller.h"

void registerRoutes(crow::SimpleApp &app, BookingController &bookingController, UserController &userController) {
    // Buildings endpoint
    CROW_ROUTE(app, "/api/buildings").methods(crow::HTTPMethod::GET)
    ([&bookingController](const crow::request &req) {
        return bookingController.getBuildings(req);
    });

    // Desks endpoint
    CROW_ROUTE(app, "/api/desks").methods(crow::HTTPMethod::GET)
    ([&bookingController](const crow::request &req) {
        return bookingController.getDesks(req);
    });

    // Bookings endpoints
    CROW_ROUTE(app, "/api/bookings").methods(crow::HTTPMethod::GET)
    ([&bookingController](const crow::request &req) {
        return bookingController.getBookings(req);
    });

    CROW_ROUTE(app, "/api/bookings").methods(crow::HTTPMethod::POST)
    ([&bookingController](const crow::request &req) {
        return bookingController.addBooking(req);
    });

    CROW_ROUTE(app, "/api/bookings/<int>").methods(crow::HTTPMethod::DELETE)
    ([&bookingController](int bookingId) {
        return bookingController.cancelBooking(bookingId);
    });

    // User endpoints
    CROW_ROUTE(app, "/api/users").methods(crow::HTTPMethod::GET)
    ([&userController](const crow::request &req) {
        return userController.getUsers(req);
    });

    CROW_ROUTE(app, "/api/users/<int>").methods(crow::HTTPMethod::GET)
    ([&userController](int userId) {
        return userController.getUser(userId);
    });

    CROW_ROUTE(app, "/api/users/register").methods(crow::HTTPMethod::POST)
    ([&userController](const crow::request &req) {
        return userController.registerUser(req);
    });

    CROW_ROUTE(app, "/api/users/login").methods(crow::HTTPMethod::POST)
    ([&userController](const crow::request &req) {
        return userController.loginUser(req);
    });

    CROW_ROUTE(app, "/api/users/<int>").methods(crow::HTTPMethod::PUT)
    ([&userController](const crow::request &req, int userId) {
        return userController.updateUser(userId, req);
    });

    CROW_ROUTE(app, "/api/users/<int>").methods(crow::HTTPMethod::DELETE)
    ([&userController](int userId) {
        return userController.deleteUser(userId);
    });

    LOG_INFO("API routes registered successfully");
}
