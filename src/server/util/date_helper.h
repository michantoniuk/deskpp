// src/server/util/date_helper.h
#ifndef DATE_HELPER_H
#define DATE_HELPER_H

#include <string>

class DateHelper {
public:
    // Check if string is a valid date in format YYYY-MM-DD
    static bool isValidDate(const std::string &dateStr);

    // Compare dates (works with YYYY-MM-DD format)
    static bool isDateBefore(const std::string &date1, const std::string &date2);

    static bool isDateAfterOrEqual(const std::string &date1, const std::string &date2);

    // Get current date in YYYY-MM-DD format
    static std::string getCurrentDate();
};

#endif // DATE_HELPER_H
