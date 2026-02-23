#ifndef TIME_HPP
#define TIME_HPP

#include <string>
#include <optional>
#include <iostream>

struct Date {
    int day;
    int month;
    int year;

    bool operator==(const Date& other) const;

    static std::optional<Date> parseDate(const std::string& s);
    static std::string getDate();

    void addDay();
    std::string toString();
};

std::ostream& operator<<(std::ostream& os, const Date& d);


#endif // TIME_HPP