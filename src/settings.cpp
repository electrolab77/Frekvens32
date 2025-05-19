#include "settings.h"

const uint8_t Settings::VALID_PPQN[11] = {1,2,4,8,12,16,24,32,48,64,96};

Settings::Settings() {
    setDefaults();
}

uint32_t Settings::calculateChecksum() {
    const uint8_t* p = (const uint8_t*)&data;
    uint32_t checksum = 0;
    for (size_t i = 0; i < sizeof(SettingsData) - sizeof(uint32_t); i++) {
        checksum += p[i];
    }
    return checksum;
}

void Settings::setDefaults() {
    data.ledIntensity = 30; // 30%
    data.scrollSpeed  = 80; // MD
    data.timeFormat   = 24; // 24H
    data.pulsePPQN    = 24; // 24 PPQN
}

void Settings::loadSettings() {
    EEPROM.begin(512);
    EEPROM.get(EEPROM_START_ADDR, data);
    
    uint32_t calculatedChecksum = calculateChecksum();
    if (calculatedChecksum != data.checksum) {
        setDefaults();
        saveSettings();
    }
    EEPROM.end();
}

void Settings::saveSettings() {
    EEPROM.begin(512);
    data.checksum = calculateChecksum();
    EEPROM.put(EEPROM_START_ADDR, data);
    EEPROM.commit();
    EEPROM.end();
}

uint8_t Settings::getLedIntensity() { return data.ledIntensity; }
uint8_t Settings::getScrollSpeed() { return data.scrollSpeed; }
uint8_t Settings::getTimeFormat() { return data.timeFormat; }
uint8_t Settings::getPulsePPQN() { return data.pulsePPQN; }

String Settings::getScrollSpeedText() {
    switch(data.scrollSpeed) {
        case 0: return "SL";  // Slow
        case 1: return "MD";  // Moderate
        case 2: return "FS";  // Fast
        default: return "MD"; // Moderate
    }
}

void Settings::nextValue(uint8_t option) {
    switch(option) {
        case OPTION_LED_INTENSITY:
            // Increment LED intensity by 10, range 10-99
            data.ledIntensity += 10;
            if (data.ledIntensity == 100) {
                data.ledIntensity = 99;
            }
            else if (data.ledIntensity > 100 || data.ledIntensity < 10) {
                data.ledIntensity = 10;
            }
            break;

        case OPTION_SCROLL_SPEED:
            // Cycle through speeds (0=Slow, 1=Medium, 2=Fast)
            data.scrollSpeed = (data.scrollSpeed + 1) % 3;
            break;

        case OPTION_TIME_FORMAT:
            // Toggle between 12/24h
            data.timeFormat = (data.timeFormat == 12) ? 24 : 12;
            break;

        case OPTION_PULSE_PPQN:
            // Find current PPQN index
            int currentIndex = 0;
            for(int i = 0; i < PPQN_COUNT; i++) {
                if(VALID_PPQN[i] == data.pulsePPQN) {
                    currentIndex = i;
                    break;
                }
            }
            // Move to next valid PPQN
            currentIndex = (currentIndex + 1) % PPQN_COUNT;
            data.pulsePPQN = VALID_PPQN[currentIndex];
            break;
    }
    
    // Save settings after any change
    //saveSettings();
}

unsigned long Settings::getScrollDelay() {
    switch(data.scrollSpeed) {
        case 0: return 120; // SL
        case 1: return 80;  // MD
        case 2: return 50;  // FS
        default: return 100;
    }
}