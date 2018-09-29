//
// DT module
// Created by sju on 24.09.18.
//

#include <DateTime.h>

#include "DateTime.h"


namespace DT {

    Time::Time(const std::uint16_t h, const std::uint16_t m, const std::uint16_t s) {
        try {
            if (checkInRange(h, 0, 23))
                hour = h;
            else
                throw ("Hour val must be between 0 and 23");

            if (checkInRange(m, 0, 59))
                minute = m;
            else
                throw ("Minute val must be between 0 and 60");

            if (checkInRange(s, 0, 60))
                seconds = s;
            else
                throw ("Second value must be between 0 and 60");

        } catch (const char *msg) {
            std::cerr << msg << '\n';
        }
    }

    double Time::getFrac() {
        double days = seconds;
        days = minute + days / 60.0;
        days = hour + days / 60.0;
        return days / 24.0;
    }


    bool validateDate(const std::uint16_t d, const std::uint16_t m, const std::uint16_t y) {
        if (y >= 0 && y <= 9999) {
            //check month
            if (m >= 1 && m <= 12) {
                //check days
                if ((d >= 1 && d <= 31) && (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12))
                    return true;
                else if ((d >= 1 && d <= 30) && (m == 4 || m == 6 || m == 9 || m == 11))
                    return true;
                else if ((d >= 1 && d <= 28) && (m == 2))
                    return true;
                    // Leap year check.
                else if (d == 29 && m == 2) {
                    if (1 && (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0)))
                        return true;
                    else {
                        //throw("Day is invalid.\n");
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                //throw("Month is not valid.\n");
                return false;
            }
        } else {
            //throw("Year is not valid.\n");
            return false;
        }

    }

    std::int32_t getDayOfWeek(std::uint16_t d, std::uint16_t m, std::uint16_t y) {
        if (!validateDate(d, m, y)) {
            return -1;
        }
        // 1990, Michael Keith and Tom Craver expression.
        std::int32_t day = (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;
        // To make Sun week day number 7 not 0.
        return (day == 0 ? 7 : day);
    }

    bool checkInRange(const std::uint16_t val, const std::uint16_t a, const std::uint16_t b) {
        return (val >= a && val <= b);
    }

    Date::Date(const std::uint16_t d, const std::uint16_t m, const std::uint16_t y) {
        if (validateDate(d, m, y)) {
            month = m;
            day = d;
            year = y;
        } else
            throw ("Invalid Date\n");
    }

    void DateTime::print() const {
        using std::cout;
        cout << toDayOfWeek(getDayOfWeek(date.Day(), date.Month(), date.Year())) << " ";
        cout << date.Day() << "/" << date.Month() << "/" << date.Year() << " ";
        cout << time.Hour() << ":" << time.Minute() << ":";
        if (time.Seconds() < 10)
            cout << "0";
        cout << time.Seconds() << '\n';
    }

    std::int32_t DateTime::dayOfWeek() const {
        return DT::getDayOfWeek(date.Day(), date.Month(), date.Year());
    }

    DateTime &DateTime::operator=(const DateTime &rsv) {
        date = Date(rsv.Day(), rsv.Month(), rsv.Year());
        return *this;
    }

    std::string toDayOfWeek(const std::int32_t day) {
        switch (day) {
            case -1:
                return "INVALID";
            case 1:
                return "Mon";
            case 2:
                return "Tue";
            case 3:
                return "Wed";
            case 4:
                return "Thu";
            case 5:
                return "Fri";
            case 6:
                return "Sat";
            case 7:
                return "Sun";
            default:
                return "INVALID";
        }
    }
}