#include "time.hpp"

#include <format>
#include <sstream>
#include <stdexcept>
#include <iomanip> 
#include <ctime>

std::string Date::getDate() {
    std::time_t t = std::time(nullptr);
    std::tm local{};

    #if defined(_WIN32) || defined(_WIN64)
        localtime_s(&local, &t);
    #else
        localtime_r(&t, &local);
    #endif

    std::string todayStr = std::format("{:02d}/{:02d}/{:04d}", local.tm_mday, local.tm_mon + 1, local.tm_year + 1900); 
    return todayStr;
}

std::optional<Date> Date::parseDate(const std::string& s) {
    Date d{};
    char sep1;
    char sep2;
    std::istringstream iss(s);
    if (!(iss >> d.day >> sep1 >> d.month >> sep2 >> d.year))
        return std::nullopt;
    if (sep1 != '/' || sep2 != '/')
        return std::nullopt;
    return d;
}

bool Date::operator==(const Date &other) const {
    return day == other.day && month == other.month && year == other.year;
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
    os << std::setw(2) << std::setfill('0') << d.day << '/'
       << std::setw(2) << std::setfill('0') << d.month << '/'
       << d.year;
    return os;
}

void Date::addDay() {
    this->day++;
}

std::string Date::toString() {
    return std::format("{:02d}/{:02d}/{:04d}", day, month, year);
}
