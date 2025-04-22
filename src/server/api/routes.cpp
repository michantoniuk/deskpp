#include "routes.h"

void registerRoutes(crow::SimpleApp &app, BookingController &bookingController,
                    UserController &userController) {
    // Endpoint budynków
    CROW_ROUTE(app, "/api/buildings").methods(crow::HTTPMethod::GET)
    ([&bookingController](const crow::request &req) {
        return bookingController.getBuildings(req);
    });

    // Endpoint biurek
    CROW_ROUTE(app, "/api/desks").methods(crow::HTTPMethod::GET)
    ([&bookingController](const crow::request &req) {
        return bookingController.getDesks(req);
    });

    // Endpointy rezerwacji
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

    // Endpointy użytkowników
    CROW_ROUTE(app, "/api/users/register").methods(crow::HTTPMethod::POST)
    ([&userController](const crow::request &req) {
        return userController.registerUser(req);
    });

    CROW_ROUTE(app, "/api/users/login").methods(crow::HTTPMethod::POST)
    ([&userController](const crow::request &req) {
        return userController.loginUser(req);
    });

    CROW_ROUTE(app, "/api/buildings/<int>/floors").methods(crow::HTTPMethod::GET)
    ([&bookingController](int buildingId) {
        return bookingController.getFloorsByBuilding(buildingId);
    });
}
