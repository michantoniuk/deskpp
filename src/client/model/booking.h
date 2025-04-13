#ifndef CLIENT_MODEL_BOOKING_H
#define CLIENT_MODEL_BOOKING_H

#include <QDate>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Booking {
public:
    Booking();
    Booking(int id, int deskId, int userId, const QDate& dateFrom, const QDate& dateTo);

    bool containsDate(const QDate& date) const;
    bool overlapsWithPeriod(const QDate& dateFrom, const QDate& dateTo) const;
    std::string toJson() const;
    static Booking fromJson(const std::string& jsonStr);

    // Getters
    int getId() const { return _id; }
    int getDeskId() const { return _deskId; }
    int getUserId() const { return _userId; }
    QDate getDateFrom() const { return _dateFrom; }
    QDate getDateTo() const { return _dateTo; }

    // Setters
    void setId(int id) { _id = id; }
    void setDeskId(int deskId) { _deskId = deskId; }
    void setUserId(int userId) { _userId = userId; }
    void setDateFrom(const QDate& dateFrom) { _dateFrom = dateFrom; }
    void setDateTo(const QDate& dateTo) { _dateTo = dateTo; }

private:
    int _id;
    int _deskId;
    int _userId;
    QDate _dateFrom;
    QDate _dateTo;
};

#endif // CLIENT_MODEL_BOOKING_H