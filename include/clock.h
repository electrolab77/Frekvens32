#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

class Clock {
  private:
    RTC_DS3231 rtc;
    bool is24Hour;

    // Convert a number to a 2-digit string
    String twoDigits(uint8_t number);

  public:
    Clock();

    // Initialize RTC, adjust if power was lost
    bool begin();

    // Set 12/24h format
    void setTimeFormat(uint8_t format);

    // Get time in HHMM format with specified format (12 or 24)
    String getTimeString(uint8_t format);

    // Get time in HHMM format with current format
    String getTimeString();

    // Get date in DDMM format
    String getDateString();

    // Get year in YYYY format
    String getYearString();

    // Update date and time
    void setDateTime(uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second);

    // Get RTC temperature
    float getTemperature();

    // Check if RTC is responding
    bool isRunning();

    // Get current DateTime object
    DateTime now();

    // Get date/time components separately
    uint16_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();

    // Check if RTC has lost power
    bool hasLostPower();

    // Force RTC update with current time
    void forceUpdate();
};

#endif