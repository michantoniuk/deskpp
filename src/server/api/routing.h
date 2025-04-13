#ifndef ROUTING_H
#define ROUTING_H

#include <crow.h>
#include "booking_controller.h"

void registerRoutes(crow::SimpleApp &app, BookingController &controller);

#endif // ROUTING_H
