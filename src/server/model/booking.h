#ifndef SERVER_MODEL_BOOKING_H
#define SERVER_MODEL_BOOKING_H

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono;

class Booking {
public:
    Booking();

    Booking(int id, int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    bool containsDate(const std::string &date) const;

    bool overlapsWithPeriod(const std::string &dateFrom, const std::string &dateTo) const;

    json toJson() const;

    static Booking fromJson(const json &j);

    // Getters
    int getId() const { return _id; }
    int getDeskId() const { return _deskId; }
    int getUserId() const { return _userId; }
    const std::string &getDateFrom() const { return _dateFrom; }
    const std::string &getDateTo() const { return _dateTo; }

    // Setters
    void setId(int id) { _id = id; }
    void setDeskId(int deskId) { _deskId = deskId; }
    void setUserId(int userId) { _userId = userId; }
    void setDateFrom(const std::string &dateFrom) { _dateFrom = dateFrom; }
    void setDateTo(const std::string &dateTo) { _dateTo = dateTo; }

private:
    int _id;
    int _deskId;
    int _userId;
    std::string _dateFrom; // Format: "yyyy-MM-dd"
    std::string _dateTo; // Format: "yyyy-MM-dd"

    // Helper methods for date comparison
    bool isDateBefore(const std::string &date1, const std::string &date2) const;

    bool isDateAfter(const std::string &date1, const std::string &date2) const;

    bool isDateEqual(const std::string &date1, const std::string &date2) const;
};

#endif // SERVER_MODEL_BOOKING_H
