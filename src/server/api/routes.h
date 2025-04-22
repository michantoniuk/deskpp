#ifndef ROUTES_H
#define ROUTES_H

#include <crow.h>
#include "controller/booking_controller.h"
#include "controller/user_controller.h"

/**
 * @brief Rejestruje ścieżki API w aplikacji Crow
 * @param app Referencja do aplikacji Crow
 * @param bookingController Referencja do kontrolera rezerwacji
 * @param userController Referencja do kontrolera użytkowników
 */
void registerRoutes(crow::SimpleApp &app, BookingController &bookingController,
                    UserController &userController);

#endif
