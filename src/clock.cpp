#include "clock.h"

// Constructor : Initializes the default format to 24h
Clock::Clock() : is24Hour(true) {}

// Initializes RTC, adjusts date/time if power has been lost
bool Clock::begin() {
    Wire.begin();
    if (!rtc.begin()) {
        return false;
    }

    // If the RTC has lost its power supply, we update with the compilation date
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    return true;
}

// Sets the time display format (12 or 24h)
void Clock::setTimeFormat(uint8_t format) {
    is24Hour = (format == 24);
}

// Gets the time in HH:MM format according to the passed format
String Clock::getTimeString(uint8_t format) {
    DateTime now = rtc.now();
    uint8_t hour = now.hour();

    // Convert 24h to 12h if necessary
    if (format == 12) {
        if (hour == 0) hour = 12;
        else if (hour > 12) hour -= 12;
    }

    return twoDigits(hour) + twoDigits(now.minute());
}

// Gets the time with the currently set format
String Clock::getTimeString() {
    return getTimeString(is24Hour ? 24 : 12);
}

// Gets the date in DDMM format
String Clock::getDateString() {
    DateTime now = rtc.now();
    return twoDigits(now.day()) + twoDigits(now.month());
}

// Gets the year as a String
String Clock::getYearString() {
    DateTime now = rtc.now();
    return String(now.year());
}

// Manually set the date and time
void Clock::setDateTime(uint16_t year, uint8_t month, uint8_t day,
                        uint8_t hour, uint8_t minute, uint8_t second) {
    rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

// Reads the internal temperature of the RTC module
float Clock::getTemperature() {
    return rtc.getTemperature();
}

// Check if the RTC is still working
bool Clock::isRunning() {
    return rtc.begin();
}

// Returns the current DateTime object
DateTime Clock::now() {
    return rtc.now();
}

// Access to individual date/time components
uint16_t Clock::getYear() { return rtc.now().year(); }
uint8_t Clock::getMonth() { return rtc.now().month(); }
uint8_t Clock::getDay() { return rtc.now().day(); }
uint8_t Clock::getHour() { return rtc.now().hour(); }
uint8_t Clock::getMinute() { return rtc.now().minute(); }
uint8_t Clock::getSecond() { return rtc.now().second(); }

// Check if the RTC has lost power
bool Clock::hasLostPower() {
    return rtc.lostPower();
}

// Rewrites the current date/time to the RTC to force a sync
void Clock::forceUpdate() {
    DateTime now = rtc.now();
    rtc.adjust(now);
}

// Formats a number to 2 digits (eg: 3 -> "03")
String Clock::twoDigits(uint8_t number) {
    if (number < 10) {
        return "0" + String(number);
    }
    return String(number);
}