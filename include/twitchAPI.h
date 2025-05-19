#ifndef TWITCH_API_H
#define TWITCH_API_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials.h" 

class TwitchAPI {
private:
    String accessToken = "";
    unsigned long lastUpdateTime;
    static const unsigned long UPDATE_INTERVAL  = 30000; // = 300000; // 5 minutes in milliseconds
    static const unsigned long DISPLAY_DURATION = 10000; // = 30000; // 30 seconds in milliseconds
    
    bool enabled;
    String streamTitle;
    String streamerName;
    String gameName;
    bool isLive;

    bool getAccessToken();
    bool updateStreamInfo();
    bool updateChannelInfo();

public:
    TwitchAPI();
    void begin();
    void enable();
    void disable();
    bool isEnabled() const { return enabled; }
    bool isStreaming() const { return isLive; }
    bool update();

    //Getters
    String getCurrentTitle() const { return streamTitle; }
    String getStreamerName() const { return streamerName; }
    String getGameName() const { return gameName; }
    String getStatusText() const;
};

#endif