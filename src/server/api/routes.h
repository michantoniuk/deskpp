#ifndef SERVER_ROUTES_H
#define SERVER_ROUTES_H

#include <crow.h>
#include "booking_controller.h"
#include "user_controller.h"

/**
 * Register all API routes with the Crow application
 */
void registerRoutes(crow::SimpleApp & app, BookingController & bookingController, UserController & userController);

#endif // SERVER_ROUTES_H
