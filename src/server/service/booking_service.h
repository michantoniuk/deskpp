#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "service.h"
#include "../repository/building_repository.h"
#include "../repository/desk_repository.h"
#include "../repository/booking_repository.h"

/**
 * @class BookingService
 * @brief Serwis obsługujący operacje związane z rezerwacjami.
 *
 * Zapewnia funkcje zarządzania budynkami, biurkami i rezerwacjami.
 */
class BookingService : public Service<Booking> {
public:
    /**
     * @brief Konstruktor
     * @param buildingRepository Referencja do repozytorium budynków
     * @param deskRepository Referencja do repozytorium biurek
     * @param bookingRepository Referencja do repozytorium rezerwacji
     */
    BookingService(BuildingRepository &buildingRepository, DeskRepository &deskRepository,
                   BookingRepository &bookingRepository);

    /**
     * @brief Pobiera wszystkie budynki
     * @return Obiekt JSON z listą budynków
     */
    json getAllBuildings();

    /**
     * @brief Pobiera wszystkie biurka
     * @return Obiekt JSON z listą biurek
     */
    json getAllDesks();

    /**
     * @brief Pobiera biurka dla wybranego budynku
     * @param buildingId Identyfikator budynku
     * @return Obiekt JSON z listą biurek
     */
    json getDesksByBuilding(int buildingId);

    /**
     * @brief Pobiera rezerwacje dla biurka w określonym okresie
     * @param deskId Identyfikator biurka
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @return Obiekt JSON z listą rezerwacji
     */
    json getBookingsForDesk(int deskId, const std::string &dateFrom, const std::string &dateTo);

    /**
     * @brief Dodaje nową rezerwację
     * @param deskId Identyfikator biurka
     * @param userId Identyfikator użytkownika
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @return Obiekt JSON z wynikiem operacji
     */
    json addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    /**
     * @brief Anuluje rezerwację
     * @param bookingId Identyfikator rezerwacji
     * @return Obiekt JSON z wynikiem operacji
     */
    json cancelBooking(int bookingId);

    /**
     * @brief Pobiera biurka dla wybranego budynku i piętra
     * @param buildingId Identyfikator budynku
     * @param floor Numer piętra
     * @return Obiekt JSON z listą biurek
     */
    json getDesksByBuildingAndFloor(int buildingId, int floor);

    /**
     * @brief Pobiera piętra dla wybranego budynku
     * @param buildingId Identyfikator budynku
     * @return Obiekt JSON z listą pięter
     */
    json getFloorsByBuilding(int buildingId);

private:
    BuildingRepository &_buildingRepo;
    DeskRepository &_deskRepo;
    BookingRepository &_bookingRepo;
};

#endif
