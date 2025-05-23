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
    
    bool enabled; // Twitch mode is active
    bool isLive;  // Stream is live
    bool getAccessToken();
    bool updateStreamInfo();
    bool updateChannelInfo();
    uint8_t currentInfoIndex = 0;
    static const uint8_t INFO_COUNT = 6; // Nombre total d'infos à faire défiler

    String streamTitle;  // Stream Title
    String streamerName; // Streamer Name
    String startTime;    // Stream start time
    String customText1 = "GRAB YOUR BEER AND CHILL WITH US";
    String customText2 = "REPLAYS ON MIXCLOUD / GIAN77";
    String customText3 = "THIS IS A JOURNEY INTO SOUND";

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
    String getStreamerName() const { return TWITCH_BROADCASTER_ID; }
    String getStreamUptime() const;
    String getNextStatusText();    
};

#endif