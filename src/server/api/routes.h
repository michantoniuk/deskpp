#ifndef ROUTES_H
#define ROUTES_H

#include <crow.h>
#include "controller/booking_controller.h"
#include "controller/user_controller.h"

void registerRoutes(crow::SimpleApp &app, BookingController &bookingController,
                    UserController &userController);

#endif
