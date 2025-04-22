#ifndef BOOKING_CONTROLLER_H
#define BOOKING_CONTROLLER_H

#include "controller.h"
#include "../../service/booking_service.h"

/**
 * @class BookingController
 * @brief Kontroler do zarządzania rezerwacjami biurek.
 *
 * Obsługuje żądania HTTP związane z budynkami, biurkami i rezerwacjami,
 * przekazując je do odpowiednich serwisów.
 */
class BookingController : public Controller {
public:
    /**
     * @brief Konstruktor
     * @param bookingService Referencja do serwisu rezerwacji
     */
    explicit BookingController(BookingService &bookingService);

    /**
     * @brief Obsługuje żądanie pobrania budynków
     * @param req Żądanie HTTP
     * @return Odpowiedź HTTP z listą budynków
     */
    crow::response getBuildings(const crow::request &req);

    /**
     * @brief Obsługuje żądanie pobrania biurek
     * @param req Żądanie HTTP
     * @return Odpowiedź HTTP z listą biurek
     */
    crow::response getDesks(const crow::request &req);

    /**
     * @brief Obsługuje żądanie pobrania rezerwacji
     * @param req Żądanie HTTP
     * @return Odpowiedź HTTP z listą rezerwacji
     */
    crow::response getBookings(const crow::request &req);

    /**
     * @brief Obsługuje żądanie dodania rezerwacji
     * @param req Żądanie HTTP
     * @return Odpowiedź HTTP z wynikiem operacji
     */
    crow::response addBooking(const crow::request &req);

    /**
     * @brief Obsługuje żądanie anulowania rezerwacji
     * @param bookingId Identyfikator rezerwacji
     * @return Odpowiedź HTTP z wynikiem operacji
     */
    crow::response cancelBooking(int bookingId);

    /**
     * @brief Obsługuje żądanie pobrania pięter dla budynku
     * @param buildingId Identyfikator budynku
     * @return Odpowiedź HTTP z listą pięter
     */
    crow::response getFloorsByBuilding(int buildingId);

private:
    BookingService &_bookingService;
};

#endif
