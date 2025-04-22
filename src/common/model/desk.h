#ifndef DESK_H
#define DESK_H

#include "entity.h"
#include "booking.h"
#include <vector>

/**
 * @class Desk
 * @brief Klasa reprezentująca biurko w systemie.
 *
 * Przechowuje informacje o biurku, w tym jego nazwę, budynek, piętro
 * oraz rezerwacje przypisane do tego biurka.
 */
class Desk : public Entity {
public:
    /**
     * @brief Konstruktor domyślny
     */
    Desk() = default;

    /**
     * @brief Konstruktor z parametrami
     * @param id Identyfikator biurka
     * @param name Nazwa biurka
     * @param buildingId Identyfikator budynku
     * @param floor Numer piętra (domyślnie 1)
     */
    Desk(int id, const std::string &name, int buildingId, int floor = 1);

    /**
     * @brief Konwertuje obiekt na format JSON
     * @return Reprezentacja JSON obiektu
     */
    json toJson() const override;

    /**
     * @brief Konwertuje obiekt na string
     * @return Tekstowa reprezentacja obiektu
     */
    std::string toString() const override;

    /**
     * @brief Sprawdza czy biurko jest dostępne (nie ma żadnych rezerwacji)
     * @return Czy biurko jest dostępne
     */
    bool isAvailable() const { return _bookings.empty(); }

    /**
     * @brief Sprawdza czy biurko jest dostępne w określonym dniu
     * @param date Data do sprawdzenia
     * @return Czy biurko jest dostępne w podanym dniu
     */
    bool isAvailableOn(const QDate &date) const;

    /**
     * @brief Sprawdza czy biurko jest dostępne w określonym okresie
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @return Czy biurko jest dostępne w podanym okresie
     */
    bool isAvailableForPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    /**
     * @brief Sprawdza czy istnieje nakładająca się rezerwacja w określonym okresie
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @return Czy istnieje nakładająca się rezerwacja
     */
    bool hasOverlappingBooking(const QDate &dateFrom, const QDate &dateTo) const;

    /**
     * @brief Pobiera wszystkie rezerwacje biurka
     * @return Wektor rezerwacji
     */
    const std::vector<Booking> &getBookings() const { return _bookings; }

    /**
     * @brief Pobiera rezerwację dla określonej daty
     * @param date Data do sprawdzenia
     * @return Obiekt rezerwacji (pusty, jeśli brak)
     */
    Booking getBookingForDate(const QDate &date) const;

    /**
     * @brief Pobiera rezerwacje po określonej dacie
     * @param date Data graniczna
     * @return Wektor rezerwacji posortowany po datach
     */
    std::vector<Booking> getBookingsAfterDate(const QDate &date) const;

    /**
     * @brief Pobiera rezerwacje zawierające określoną datę
     * @param date Data do sprawdzenia
     * @return Wektor rezerwacji zawierających datę
     */
    std::vector<Booking> getBookingsContainingDate(const QDate &date) const;

    /**
     * @brief Dodaje nową rezerwację na podstawie dat
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     * @param bookingId Identyfikator rezerwacji (opcjonalny)
     */
    void addBooking(const QDate &dateFrom, const QDate &dateTo, int bookingId = 0);

    /**
     * @brief Dodaje istniejącą rezerwację
     * @param booking Obiekt rezerwacji
     */
    void addBooking(const Booking &booking);

    /**
     * @brief Anuluje rezerwację o określonym identyfikatorze
     * @param bookingId Identyfikator rezerwacji
     */
    void cancelBooking(int bookingId);

    /**
     * @brief Anuluje wszystkie rezerwacje biurka
     */
    void cancelAllBookings() { _bookings.clear(); }

    /**
     * @brief Pobiera nazwę biurka
     * @return Nazwa biurka
     */
    const std::string &getName() const { return _name; }

    /**
     * @brief Pobiera identyfikator budynku
     * @return Identyfikator budynku
     */
    int getBuildingId() const { return _buildingId; }

    /**
     * @brief Pobiera numer piętra
     * @return Numer piętra
     */
    int getFloor() const { return _floor; }

    /**
     * @brief Ustawia nazwę biurka
     * @param name Nowa nazwa biurka
     */
    void setName(const std::string &name) { _name = name; }

    /**
     * @brief Ustawia identyfikator budynku
     * @param buildingId Nowy identyfikator budynku
     */
    void setBuildingId(int buildingId) { _buildingId = buildingId; }

    /**
     * @brief Ustawia numer piętra
     * @param floor Nowy numer piętra
     */
    void setFloor(int floor) { _floor = floor; }

    /**
     * @brief Sprawdza czy biurko jest zarezerwowane
     * @return Czy biurko jest zarezerwowane
     */
    bool isBooked() const { return !isAvailable(); }

    /**
     * @brief Sprawdza czy biurko jest zarezerwowane w określonym dniu
     * @param date Data do sprawdzenia
     * @return Czy biurko jest zarezerwowane w podanym dniu
     */
    bool isBookedOnDate(const QDate &date) const { return !isAvailableOn(date); }

private:
    std::string _name;
    int _buildingId;
    int _floor;
    std::vector<Booking> _bookings;

    /**
     * @brief Sortuje rezerwacje według daty
     */
    void sortBookings();
};
#endif
