#include "twitchAPI.h"
#include "defines.h"
#include "clock.h"

// Externally defined RTC clock instance
extern RTC_DS3231 rtcClock;

TwitchAPI::TwitchAPI() : enabled(false), currentInfoIndex(0) {}

void TwitchAPI::begin() {
    disable(); // Initial state is disabled
}

void TwitchAPI::enable() {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("TWITCH MODE");
    if (!enabled) {
        enabled = true;
        DEBUG_PRINTLN("  Twitch Mode : ON");
        
        // Connect to WiFi
        DEBUG_PRINT("  WiFi Connection ");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        // Wait for connection with timeout
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
            DEBUG_PRINT(".");
            delay(500);
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            DEBUG_PRINTLN(" SUCCES !");
            DEBUG_PRINTLN("    MAC : " + WiFi.macAddress());
            DEBUG_PRINTLN("    IP  : " + WiFi.localIP().toString());
            
            // Get initial stream info
            if (getAccessToken()) {
                DEBUG_PRINTLN("  Access Token : OK");
                if (updateStreamInfo()) {
                    DEBUG_PRINTLN("  Stream Info : OK");
                } else {
                    DEBUG_PRINTLN("  Stream Info : KO");
                }
            } else {
                DEBUG_PRINTLN("  Access Token : FAILED");
            }
        } else {
            DEBUG_PRINTLN(" FAILED !");
            disable(); // Disable Twitch mode if WiFi fails
        }
    }
}

void TwitchAPI::disable() {
    DEBUG_PRINTLN("  Disable Twitch Mode");
    if (enabled) {
        enabled = false;
        DEBUG_PRINTLN("    Twitch Mode : OFF");
        WiFi.disconnect(true);
        DEBUG_PRINTLN("    WiFi : OFF");
        WiFi.mode(WIFI_OFF);
        streamTitle = "";
        DEBUG_PRINTLN("    Reset Stream Title : OK");
        streamerName = "";
        DEBUG_PRINTLN("    Reset Streamer Name : OK");
        startTime = "";
        DEBUG_PRINTLN("    Reset Start Time : OK");
        isLive = false;
    } else {
        DEBUG_PRINTLN("    > Twitch Mode already OFF !");
    }
}

bool TwitchAPI::getAccessToken() {
    HTTPClient http;

    String url = "https://id.twitch.tv/oauth2/token";
    String post_data = "client_id=" + String(TWITCH_CLIENT_ID) +
                      "&client_secret=" + String(TWITCH_CLIENT_SECRET) +
                      "&grant_type=client_credentials";
    
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    int httpCode = http.POST(post_data);
    bool success = false;

    DEBUG_PRINT("  Get Access Token : ");  
    if (httpCode == 200) {
        StaticJsonDocument<1024> doc;
        deserializeJson(doc, http.getString());
        accessToken = doc["access_token"].as<String>();
        success = true;
        DEBUG_PRINTLN("SUCCESS");
    } else {
        DEBUG_PRINTLN("FAILED");
    }
    
    http.end();
    return success;
}

bool TwitchAPI::updateStreamInfo() {
    DEBUG_PRINTLN("  Update Stream Info");
    if (!getAccessToken()) return false;

    HTTPClient http;
    String url = "https://api.twitch.tv/helix/streams?user_login=" + String(TWITCH_BROADCASTER_ID);
    
    http.begin(url);
    http.addHeader("Client-ID", TWITCH_CLIENT_ID);
    http.addHeader("Authorization", "Bearer " + accessToken);
    
    int httpCode = http.GET();
    bool success = false;
    
    DEBUG_PRINT("  Update Info : ");
    if (httpCode == 200) {
        StaticJsonDocument<1024> doc;
        deserializeJson(doc, http.getString());
        
        if (doc["data"][0]) {
            isLive = true;
            streamTitle = doc["data"][0]["title"].as<String>();
            streamerName = doc["data"][0]["user_name"].as<String>();
            startTime = doc["data"][0]["started_at"].as<String>();
            success = true;
            
            DEBUG_PRINTLN("SUCCESS");
            DEBUG_PRINTLN("  Stream is LIVE");
            DEBUG_PRINTLN("  Stream Title : " + streamTitle);
            DEBUG_PRINTLN("  Streamer Name : " + streamerName);
            DEBUG_PRINTLN("  Start Time : " + startTime);
        } else {
            isLive = false;
            DEBUG_PRINTLN("FAILED");
            DEBUG_PRINTLN("  Stream is OFFLINE");
        }
    } else {
        isLive = false;
        DEBUG_PRINTLN("FAILED");
        DEBUG_PRINTLN("  Code: " + String(httpCode));
    }
    
    http.end();
    return success;
}

String TwitchAPI::getNextStatusText(StatusMode mode) {
    if (!enabled) return "OFF";
    if (!isLive) {
        currentInfoIndex = 0;
        return "STREAM OFFLINE";
    }

    String status;
    uint8_t newIndex;
    
    if (mode == STATUS_RANDOM) {
        // Get random index different from current
        do {
            newIndex = random(INFO_COUNT);
        } while (newIndex == currentInfoIndex);
    } else {
        // Get next index in cycle, skipping if same text
        do {
            newIndex = (currentInfoIndex + 1) % INFO_COUNT;
            // Get text for this index to compare
            String newText = getTextForIndex(newIndex);
            String currentText = getTextForIndex(currentInfoIndex);
            if (newText != currentText) break;
            newIndex = (newIndex + 1) % INFO_COUNT;
        } while (true);
    }
    
    currentInfoIndex = newIndex;
    return getTextForIndex(newIndex);
}

// Add new private helper method
String TwitchAPI::getTextForIndex(uint8_t index) {
    switch(index) {
        case 0: return getStreamerName();
        case 1: return customText1;
        case 2: return getCurrentTitle();
        case 3: return customText2;
        case 4: return "UPTIME " + getStreamUptime();
        case 5: return customText3;
        default: return "ERROR";
    }
}

bool TwitchAPI::update() {
    // Only check if connection is lost
    if (!enabled) return false;
    
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("  WiFi connection lost!");
        disable();
        return false;
    }
    
    return true;
}

String TwitchAPI::getStreamUptime() const {
    DEBUG_PRINTLN("    === Stream Uptime Debug ===");
    String result = "00:00:00";
    
    if (startTime.isEmpty()) {
        DEBUG_PRINTLN("    Error: Empty start time");
        return result;
    }
    DEBUG_PRINTLN("    Start time (ISO): " + startTime);
    
    // Parse ISO8601 UTC date
    struct tm startTm = {0};
    if (!strptime(startTime.c_str(), "%Y-%m-%dT%H:%M:%SZ", &startTm)) {
        DEBUG_PRINTLN("    Error: Failed to parse start time");
        return "ERROR";
    }
    
    // Convert to timestamp keeping UTC
    startTm.tm_isdst = 0;
    
    // Custom timegm implementation
    char *tz = getenv("TZ");
    setenv("TZ", "", 1); 
    tzset();
    time_t start = mktime(&startTm);
    if (tz) {
        setenv("TZ", tz, 1);
    } else {
        unsetenv("TZ");
    }
    tzset();
    
    DEBUG_PRINTLN("    Start time (UTC): " + String(start));
       
    // Get current UTC time from RTC (subtract 2 hours to convert from local to UTC)
    DateTime now = rtcClock.now();
    time_t nowTime = now.unixtime() - (2 * 3600);
    DEBUG_PRINTLN("    Current RTC time (UTC): " + String(nowTime));

    // Calculate difference
    time_t diff = nowTime - start;
    if (diff < 0) diff = 0;
    DEBUG_PRINTLN("    Time difference: " + String(diff));
    
    // Format as HH:MM:SS
    unsigned int hours = diff / 3600;
    unsigned int minutes = (diff % 3600) / 60;
    unsigned int seconds = diff % 60;
    DEBUG_PRINTLN("    Hours: " + String(hours));
    DEBUG_PRINTLN("    Minutes: " + String(minutes));
    DEBUG_PRINTLN("    Seconds: " + String(seconds));

    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", 
             hours % 100, minutes % 60, seconds % 60);
   
    result = String(buffer);
    DEBUG_PRINTLN("    Formatted: " + result);
    DEBUG_PRINTLN("    ===========================");
    
    return result;
}