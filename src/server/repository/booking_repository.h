#ifndef BOOKING_REPOSITORY_H
#define BOOKING_REPOSITORY_H

#include "sqlite_repository.h"
#include "common/model/booking.h"
#include <memory>

/**
 * @class BookingRepository
 * @brief Repozytorium do zarządzania rezerwacjami w bazie danych.
 *
 * Zapewnia operacje CRUD na rezerwacjach oraz dodatkowe funkcje
 * do wyszukiwania i weryfikacji rezerwacji.
 */
class BookingRepository : public SQLiteRepository<Booking> {
public:
    /**
     * @brief Konstruktor
     * @param db Współdzielony wskaźnik do bazy danych
     */
    explicit BookingRepository(std::shared_ptr<SQLite::Database> db);

    /**
     * @brief Wyszukuje rezerwacje dla wybranego biurka
     * @param deskId Identyfikator biurka
     * @return Wektor rezerwacji
     */
    std::vector<Booking> findByDeskId(int deskId);

    /**
     * @brief Wyszukuje rezerwacje dla wybranego użytkownika
     * @param userId Identyfikator użytkownika
     * @return Wektor rezerwacji
     */
    std::vector<Booking> findByUserId(int userId);

    /**
     * @brief Wyszukuje rezerwacje dla biurka w określonym okresie
     * @param deskId Identyfikator biurka
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @return Wektor rezerwacji
     */
    std::vector<Booking> findByDateRange(int deskId, const std::string &dateFrom,
                                         const std::string &dateTo);

    /**
     * @brief Sprawdza czy istnieje nakładająca się rezerwacja
     * @param deskId Identyfikator biurka
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @return Czy istnieje nakładająca się rezerwacja
     */
    bool hasOverlappingBooking(int deskId, const std::string &dateFrom, const std::string &dateTo);

private:
    /**
     * @brief Konwertuje wiersz z bazy na obiekt rezerwacji
     * @param query Zapytanie SQL z wynikami
     * @return Obiekt rezerwacji
     */
    static Booking bookingFromRow(SQLite::Statement &query);
};

#endif
