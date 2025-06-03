#ifndef TWITCH_API_H
#define TWITCH_API_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials.h" 

// Add enum for Display mode
enum StatusMode {
    STATUS_CYCLE,
    STATUS_RANDOM
};

class TwitchAPI {
private:
    String accessToken = "";
    unsigned long lastUpdateTime;
    unsigned long lastConnectionCheck; 
    static const unsigned long UPDATE_INTERVAL  = 30000; // = 300000; // 5 minutes in milliseconds
    static const unsigned long DISPLAY_DURATION = 10000; // = 30000; // 30 seconds in milliseconds
    static const unsigned long CONNECTION_CHECK_INTERVAL = 300000;  // 5 minutes in milliseconds
    
    bool enabled; // Twitch mode is active
    bool isLive;  // Stream is live
    bool getAccessToken();
    bool updateStreamInfo();
    bool updateChannelInfo();
    uint8_t currentInfoIndex = 0;
    static const uint8_t INFO_COUNT = 6; // Total number of informations to scroll

    // Twitch Infos
    String streamTitle;  // Stream Title
    String streamerName; // Streamer Name
    String startTime;    // Stream Start Time

    // Custom texts
    String customText1 = "GRAB YOUR BEER AND CHILL WITH US";
    String customText2 = "REPLAYS ON MIXCLOUD / GIAN77";
    String customText3 = "THIS IS A JOURNEY INTO SOUND";

    // Event tracking
    unsigned long lastEventCheck;
    static const unsigned long EVENT_CHECK_INTERVAL = 10000; // Check every 10s
    String lastFollowerId;
    String lastSubscriberId;
    String lastRaiderId;
    String lastEventMessage;
    
    String getTextForIndex(uint8_t index);

    // Event methods
    bool checkNewEvents();
    bool checkNewFollowers();
    bool checkNewSubscribers();
    bool checkNewRaids();
    String formatEventMessage(const String& type, const String& username);

public:
    TwitchAPI();
    void begin();
    void enable();
    void disable();
    bool isEnabled() const { return enabled; }
    bool isStreaming() const { return isLive; }
    bool update(bool isScrolling = false);

    //Getters
    String getCurrentTitle() const { return streamTitle; }
    String getStreamerName() const { return TWITCH_BROADCASTER_ID; }
    String getStreamUptime() const;
    String getNextStatusText(StatusMode mode = STATUS_CYCLE);    

    bool hasNewEvent() const;
    String getEventMessage();
};

#endif