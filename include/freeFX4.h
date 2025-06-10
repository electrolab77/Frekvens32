#ifndef FREE_FX4_H
#define FREE_FX4_H

#include "display.h"
#include "settings.h"

struct Position {
    int x;
    int y;
    int dx;
    int dy;
};

class FreeFX4 {
private:
    Settings* settings;
    uint8_t currentFrame;
    unsigned long lastUpdate;
    Position position;  // Add position member
    
    // Constants for logo dimensions
    static const uint8_t LOGO_WIDTH = 12;
    static const uint8_t LOGO_HEIGHT = 12;
    static const uint8_t MATRIX_SIZE = 16;
    
    unsigned long getDelay();

public:
    FreeFX4(Settings* settings);
    void begin();
    void update(Display &display);
};

#endif