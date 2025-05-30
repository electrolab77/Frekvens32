#ifndef FREE_FX1_H
#define FREE_FX1_H

#include "display.h"
#include "settings.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

class FreeFX1 {
    private:
        int16_t x = 0;              // Current X position
        int16_t y = 0;              // Current Y position
        uint8_t lastDirection = 0;  // Last direction
        unsigned long lastMove = 0; // Last move
        Settings* settings;         // Pointer to parameters

        void moveSquare();          // Next movement calculation
        unsigned long getDelay();   // Delay calculation

    public:
        FreeFX1(Settings* settings);
        void begin();
        void update(Display &display);
};

#endif