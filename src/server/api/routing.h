#ifndef ROUTING_H
#define ROUTING_H

#include <crow.h>
#include "booking_controller.h"
#include "user_controller.h"

void registerRoutes(crow::SimpleApp &app, BookingController &bookingController, UserController &userController);

#endif // ROUTING_H
