#include "wifiNTP.h"
#include "debug.h"

WifiNTP::WifiNTP(Clock* clockPtr) : rtc(clockPtr), 
                                    syncSuccess(false),
                                    gmtOffset_sec(3600),
                                    daylightOffset_sec(3600) {}

bool WifiNTP::sync() {
    DEBUG_PRINTLN("WiFi NTP Sync");
    syncSuccess = false;
    statusMessage = "";

    // 1. WiFi connection
    DEBUG_PRINT("WiFi Connexion : ");
    statusMessage = "WIFI CONNECTING";
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < wifiTimeout) {
        delay(1000);
        attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("FAILED");
        statusMessage = "WIFI FAILED";
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return false;
    }
    DEBUG_PRINTLN("OK");

    // 2. NTP Configuration
    DEBUG_PRINTLN("NTP Configuration");
    statusMessage = "NTP SYNC";
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // 3. Waiting for the hour
    DEBUG_PRINT("Get Local Time : ");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        DEBUG_PRINTLN("FAILED");
        statusMessage = "NTP FAILED";
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return false;
    }
    DEBUG_PRINTLN("OK");

    // 4. RTC Update
    DEBUG_PRINTLN("RTC Update");
    rtc->setDateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    );

    // 5. WiFi disconnection
    DEBUG_PRINTLN("WiFi Disconnect");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    syncSuccess = true;
    DEBUG_PRINTLN("Sync Success");
    statusMessage = "SYNC OK";
    return true;
}

bool WifiNTP::getSyncSuccess() {
    return syncSuccess;
}

String WifiNTP::getStatusMessage() {
    return statusMessage;
}

void WifiNTP::setWiFiCredentials(const char* newSsid, const char* newPassword) {
    ssid = newSsid;
    password = newPassword;
}

void WifiNTP::setNTPServer(const char* server) {
    ntpServer = server;
}

void WifiNTP::setTimezone(long gmtOffset, int daylightOffset) {
    gmtOffset_sec = gmtOffset;
    daylightOffset_sec = daylightOffset;
}