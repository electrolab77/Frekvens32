#ifndef WIFI_NTP_H
#define WIFI_NTP_H

#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include "credentials.h"
#include "clock.h"

class WifiNTP {
  private:
    // WiFi parameters
    const char* ssid     = WIFI_SSID;
    const char* password = WIFI_PASSWORD;

    // NTP parameters
    const char* ntpServer = "time.nist.gov";
    long gmtOffset_sec;
    int daylightOffset_sec;

    // WiFi connection timeout (in seconds)
    const int wifiTimeout = 20;

    Clock* rtc;
    bool syncSuccess;
    String statusMessage;

  public:
    // Constructor
    WifiNTP(Clock* clockPtr);

    // Start synchronization
    bool sync();

    // Get the status of the last synchronization
    bool getSyncSuccess();

    // Get the status message
    String getStatusMessage();

    // Set WiFi credentials
    void setWiFiCredentials(const char* newSsid, const char* newPassword);

    // Set the NTP server
    void setNTPServer(const char* server);

    // Set the timezone
    void setTimezone(long gmtOffset, int daylightOffset);
};

#endif