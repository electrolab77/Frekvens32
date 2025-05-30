#include "twitchAPI.h"
#include "defines.h"

TwitchAPI::TwitchAPI() : enabled(false), currentInfoIndex(0) {}

void TwitchAPI::begin() {
    disable(); // Initial state is disabled
}

void TwitchAPI::enable() {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Enable Twitch Mode");
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
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Disable Twitch Mode");
    if (enabled) {
        enabled = false;
        DEBUG_PRINTLN("  Twitch Mode : OFF");
        WiFi.disconnect(true);
        DEBUG_PRINTLN("  WiFi : OFF");
        WiFi.mode(WIFI_OFF);
        streamTitle = "";
        DEBUG_PRINTLN("  Reset Stream Title : OK");
        streamerName = "";
        DEBUG_PRINTLN("  Reset Streamer Name : OK");
        startTime = "";
        DEBUG_PRINTLN("  Reset Start Time : OK");
        isLive = false;
    } else {
        DEBUG_PRINTLN("  > Twitch Mode already OFF !");
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

String TwitchAPI::getNextStatusText() {
    if (!enabled) return "OFF";
    if (!isLive) {
        currentInfoIndex = 0;
        return "STREAM OFFLINE";
    }
    
    String status;
    switch(currentInfoIndex) {
        case 0:
            status = getStreamerName(); // Streamer Name
            break;
        case 1:
            status = customText1; //.length() > 0 ? customText1 : "CUSTOM TEXT 01";
            break;
        case 2:
            status = getCurrentTitle(); // Stream Title
            break;
        case 3:
            status = customText2; //.length() > 0 ? customText2 : "CUSTOM TEXT 02";
            break;
        case 4:
            status = "UPTIME : " + getStreamUptime(); // Uptime
            break;
        case 5:
            status = customText3; //.length() > 0 ? customText3 : "CUSTOM TEXT 03";
            break;
    }
    
    // Increment index for next time
    currentInfoIndex = (currentInfoIndex + 1) % INFO_COUNT;
    
    return status;
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
    if (startTime.isEmpty()) return "UNKNOWN";
    
    // Parse the ISO8601 date from Twitch
    struct tm tm;
    strptime(startTime.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
    time_t startTime = mktime(&tm);
    
    // Get current time
    time_t now;
    time(&now);
    
    // Calculate difference
    int diff = now - startTime;
    
    // Format uptime
    int hours = diff / 3600;
    int minutes = (diff % 3600) / 60;
    
    return String(hours) + "H" + (minutes < 10 ? "0" : "") + String(minutes);
}