// src/server/util/date_helper.cpp
#include "date_helper.h"
#include <chrono>
#include <iomanip>
#include <sstream>

bool DateHelper::isValidDate(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return !ss.fail();
}

bool DateHelper::isDateBefore(const std::string& date1, const std::string& date2) {
    return date1 < date2; // Simple string comparison works for yyyy-MM-dd format
}

bool DateHelper::isDateAfterOrEqual(const std::string& date1, const std::string& date2) {
    return date1 >= date2;
}

std::string DateHelper::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}