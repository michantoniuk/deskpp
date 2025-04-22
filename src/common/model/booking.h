#ifndef BOOKING_H
#define BOOKING_H

#include "entity.h"
#include <QDate>

/**
 * @class Booking
 * @brief Klasa reprezentująca rezerwację biurka.
 *
 * Przechowuje informacje o rezerwacji, w tym identyfikatory biurka i użytkownika,
 * oraz okres rezerwacji.
 */
class Booking : public Entity {
public:
    /**
     * @brief Konstruktor domyślny
     */
    Booking() = default;

    /**
     * @brief Konstruktor z parametrami
     * @param id Identyfikator rezerwacji
     * @param deskId Identyfikator biurka
     * @param userId Identyfikator użytkownika
     * @param dateFrom Data początkowa
     * @param dateTo Data końcowa
     */
    Booking(int id, int deskId, int userId, const QDate &dateFrom, const QDate &dateTo);

    /**
     * @brief Konstruktor z parametrami (wersja tekstowa dat)
     * @param id Identyfikator rezerwacji
     * @param deskId Identyfikator biurka
     * @param userId Identyfikator użytkownika
     * @param dateFrom Data początkowa (format: yyyy-MM-dd)
     * @param dateTo Data końcowa (format: yyyy-MM-dd)
     */
    Booking(int id, int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

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
     * @brief Sprawdza czy rezerwacja zawiera określoną datę
     * @param date Data do sprawdzenia
     * @return Czy data mieści się w okresie rezerwacji
     */
    bool containsDate(const QDate &date) const;

    /**
     * @brief Sprawdza czy rezerwacja zawiera określoną datę (wersja tekstowa)
     * @param date Data do sprawdzenia (format: yyyy-MM-dd)
     * @return Czy data mieści się w okresie rezerwacji
     */
    bool containsDate(const std::string &date) const;

    /**
     * @brief Sprawdza czy rezerwacja nakłada się z określonym okresem
     * @param dateFrom Data początkowa okresu
     * @param dateTo Data końcowa okresu
     * @return Czy okresy się nakładają
     */
    bool overlapsWithPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    /**
     * @brief Sprawdza czy rezerwacja nakłada się z określonym okresem (wersja tekstowa)
     * @param dateFrom Data początkowa okresu (format: yyyy-MM-dd)
     * @param dateTo Data końcowa okresu (format: yyyy-MM-dd)
     * @return Czy okresy się nakładają
     */
    bool overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const;

    /**
     * @brief Tworzy obiekt rezerwacji z danych JSON
     * @param j Dane JSON
     * @return Obiekt rezerwacji
     */
    static Booking fromJson(const json &j);

    /**
     * @brief Pobiera identyfikator biurka
     * @return Identyfikator biurka
     */
    int getDeskId() const { return _deskId; }

    /**
     * @brief Pobiera identyfikator użytkownika
     * @return Identyfikator użytkownika
     */
    int getUserId() const { return _userId; }

    /**
     * @brief Pobiera datę początkową
     * @return Data początkowa
     */
    QDate getDateFrom() const { return _dateFrom; }

    /**
     * @brief Pobiera datę końcową
     * @return Data końcowa
     */
    QDate getDateTo() const { return _dateTo; }

    /**
     * @brief Pobiera datę początkową jako tekst
     * @return Data początkowa w formacie yyyy-MM-dd
     */
    std::string getDateFromString() const { return _dateFrom.toString("yyyy-MM-dd").toStdString(); }

    /**
     * @brief Pobiera datę końcową jako tekst
     * @return Data końcowa w formacie yyyy-MM-dd
     */
    std::string getDateToString() const { return _dateTo.toString("yyyy-MM-dd").toStdString(); }

    /**
     * @brief Ustawia identyfikator biurka
     * @param deskId Nowy identyfikator biurka
     */
    void setDeskId(int deskId) { _deskId = deskId; }

    /**
     * @brief Ustawia identyfikator użytkownika
     * @param userId Nowy identyfikator użytkownika
     */
    void setUserId(int userId) { _userId = userId; }

    /**
     * @brief Ustawia datę początkową
     * @param dateFrom Nowa data początkowa
     */
    void setDateFrom(const QDate &dateFrom) { _dateFrom = dateFrom; }

    /**
     * @brief Ustawia datę końcową
     * @param dateTo Nowa data końcowa
     */
    void setDateTo(const QDate &dateTo) { _dateTo = dateTo; }

    /**
     * @brief Ustawia datę początkową (wersja tekstowa)
     * @param dateFrom Nowa data początkowa (format: yyyy-MM-dd)
     */
    void setDateFrom(const std::string &dateFrom);

    /**
     * @brief Ustawia datę końcową (wersja tekstowa)
     * @param dateTo Nowa data końcowa (format: yyyy-MM-dd)
     */
    void setDateTo(const std::string &dateTo);

private:
    int _deskId = 0, _userId = 0;
    QDate _dateFrom, _dateTo;
};
#endif
