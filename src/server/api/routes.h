#ifndef SERVER_ROUTES_H
#define SERVER_ROUTES_H

#include <crow.h>

#include "controller/admin_controller.h"
#include "controller/booking_controller.h"
#include "controller/user_controller.h"

/**
 * Register all API routes with the Crow application
 */
void registerRoutes(crow::SimpleApp &app, BookingController &bookingController, UserController &userController,
                    AdminController &adminController);

#endif // SERVER_ROUTES_H
