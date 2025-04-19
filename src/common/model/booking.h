#ifndef BOOKING_H
#define BOOKING_H

#include "entity.h"
#include <string>
#include <QDate>

class Booking : public Entity {
public:
    Booking() = default;

    Booking(int id, int deskId, int userId, const QDate &dateFrom, const QDate &dateTo);

    Booking(int id, int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    json toJson() const override;

    std::string toString() const override;

    // Core functionality
    bool containsDate(const QDate &date) const;

    bool containsDate(const std::string &date) const;

    bool overlapsWithPeriod(const QDate &dateFrom, const QDate &dateTo) const;

    bool overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const;

    static Booking fromJson(const json &j);

    // Getters/setters
    int getDeskId() const { return _deskId; }
    int getUserId() const { return _userId; }
    QDate getDateFrom() const { return _dateFrom; }
    QDate getDateTo() const { return _dateTo; }

    std::string getDateFromString() const { return _dateFrom.toString("yyyy-MM-dd").toStdString(); }
    std::string getDateToString() const { return _dateTo.toString("yyyy-MM-dd").toStdString(); }

    void setDeskId(int deskId) { _deskId = deskId; }
    void setUserId(int userId) { _userId = userId; }
    void setDateFrom(const QDate &dateFrom) { _dateFrom = dateFrom; }
    void setDateTo(const QDate &dateTo) { _dateTo = dateTo; }

    void setDateFrom(const std::string &dateFrom);

    void setDateTo(const std::string &dateTo);

private:
    int _deskId = 0;
    int _userId = 0;
    QDate _dateFrom;
    QDate _dateTo;
};

#endif // BOOKING_H
