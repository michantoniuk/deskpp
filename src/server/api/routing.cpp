#include "routing.h"
#include "../util/logger.h"

void registerRoutes(crow::SimpleApp &app, BookingController &controller) {
    // Buildings endpoint
    CROW_ROUTE(app, "/api/buildings")
            .methods(crow::HTTPMethod::GET)
            ([&controller](const crow::request &req) {
                return controller.getBuildings(req);
            });

    // Desks endpoint
    CROW_ROUTE(app, "/api/desks")
            .methods(crow::HTTPMethod::GET)
            ([&controller](const crow::request &req) {
                return controller.getDesks(req);
            });

    // Bookings endpoints
    CROW_ROUTE(app, "/api/bookings")
            .methods(crow::HTTPMethod::GET)
            ([&controller](const crow::request &req) {
                return controller.getBookings(req);
            });

    CROW_ROUTE(app, "/api/bookings")
            .methods(crow::HTTPMethod::POST)
            ([&controller](const crow::request &req) {
                return controller.addBooking(req);
            });

    CROW_ROUTE(app, "/api/bookings/<int>")
            .methods(crow::HTTPMethod::DELETE)
            ([&controller](int bookingId) {
                return controller.cancelBooking(bookingId);
            });

    LOG_INFO("API routes registered");
}
