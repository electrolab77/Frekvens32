#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

// Options of the menu
enum SettingsOption {
  OPTION_CASE_TEMP,
  OPTION_ESP32_TEMP, 
  OPTION_LED_INTENSITY,
  OPTION_SCROLL_SPEED,
  OPTION_FX_SPEED,
  OPTION_PULSE_PPQN,
  OPTION_TIME_FORMAT,
  OPTION_SYNC_NTP,
  OPTION_TWITCH_MODE,
  OPTION_DEMO_MODE,
  OPTION_COUNT
};

class Settings {
  private:
    struct SettingsData {
      uint8_t ledIntensity; // 10-99%
      uint8_t scrollSpeed;  // 0=SL, 1=MD, 2=FS
      uint8_t fxSpeed;      // 10-99
      uint8_t timeFormat;   // 12/24
      uint8_t pulsePPQN;    // 1,2,4,8,12,16,24,32,48,64,96
      uint32_t checksum;    // For validation
    };
    
    SettingsData data;
    static const int EEPROM_START_ADDR = 0;
    
    static const uint8_t VALID_PPQN[11];
    static const uint8_t PPQN_COUNT = 11;
    
    static const boolean DEBUG_MODE = true;

    uint32_t calculateChecksum();
    void setDefaults();
    
  public:
    Settings();
    void loadSettings();
    void saveSettings();
    
    uint8_t getLedIntensity();
    
    uint8_t getScrollSpeed();
    unsigned long getScrollDelay();
    String getScrollSpeedText();

    uint8_t getTimeFormat();
    
    uint8_t getPulsePPQN();
    uint8_t getFxSpeed();
    unsigned long getFxDelay();
    
    void nextValue(uint8_t option);
};

#endif