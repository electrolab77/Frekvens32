#include "wifiNTP.h"

WifiNTP::WifiNTP(Clock* clockPtr) : rtc(clockPtr), 
                                    syncSuccess(false),
                                    gmtOffset_sec(3600),
                                    daylightOffset_sec(3600) {}

bool WifiNTP::sync() {
    syncSuccess = false;
    statusMessage = "";

    // 1. WiFi connection
    statusMessage = "WIFI CONNECTING";
    Serial.println("WIFI CONNECTING"); // ### DEBUG ###
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < wifiTimeout) {
        delay(1000);
        attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        statusMessage = "WIFI FAILED";
        Serial.println("WIFI FAILED"); // ### DEBUG ###
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return false;
    }

    // 2. NTP configuration
    statusMessage = "NTP SYNC";
    Serial.println("NTP SYNC"); // ### DEBUG ###
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // 3. Waiting for the hour
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        statusMessage = "NTP FAILED";
        Serial.println("NTP FAILED"); // ### DEBUG ###
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return false;
    }

    // 4. RTC Update
    rtc->setDateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    );

    // 5. WiFi disconnection
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    syncSuccess = true;
    statusMessage = "SYNC OK";
    Serial.println("SYNC OK"); // ### DEBUG ###
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