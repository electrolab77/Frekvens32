#include "freeFX4.h"
#include "defines.h"

// Twitch logo (12x12) - moved to class as static const member
static const uint8_t logoTwitch[12][12] = {
    {0,0,1,1,1,1,1,1,1,1,1,1},
    {0,1,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,1,0,0,1,0,0,1},
    {1,1,0,0,0,1,0,0,1,0,0,1},
    {1,1,0,0,0,1,0,0,1,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,1,0},
    {1,1,1,1,0,0,1,1,1,1,0,0},
    {1,1,1,1,0,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,1,1,1,0,0,0,0,0,0,0}
};

FreeFX4::FreeFX4(Settings* settings) {
    this->settings = settings;
    currentFrame = 0;
    lastUpdate = 0;
    
    // Initialize position using class constants
    position.x = random(0, MATRIX_SIZE - LOGO_WIDTH + 1);
    position.y = random(0, MATRIX_SIZE - LOGO_HEIGHT + 1);
    position.dx = 1;
    position.dy = 1;
}

void FreeFX4::begin() {
    currentFrame = 0;
    lastUpdate = millis();
}

unsigned long FreeFX4::getDelay() {
    uint8_t speed = settings->getFxSpeed();
    return map(speed, 10, 99, 200, 50); // Faster movement: 200ms to 50ms
}

void FreeFX4::update(Display &display) {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdate >= getDelay()) {
        lastUpdate = currentTime;
        
        // Update position
        position.x += position.dx;
        position.y += position.dy;
        
        // Bounce off walls using class constants
        if (position.x <= 0 || position.x >= MATRIX_SIZE - LOGO_WIDTH) {
            position.dx *= -1;
            position.x = constrain(position.x, 0, MATRIX_SIZE - LOGO_WIDTH);
        }
        if (position.y <= 0 || position.y >= MATRIX_SIZE - LOGO_HEIGHT) {
            position.dy *= -1;
            position.y = constrain(position.y, 0, MATRIX_SIZE - LOGO_HEIGHT);
        }
        
        // Add some randomness to direction (kept as is)
        if (random(100) < 5) {
            position.dx = random(2) * 2 - 1;
            position.dy = random(2) * 2 - 1;
        }
        
        // Clear and draw
        display.clear();
        
        // Draw Twitch logo using class constants
        for (int y = 0; y < LOGO_HEIGHT; y++) {
            for (int x = 0; x < LOGO_WIDTH; x++) {
                if (logoTwitch[y][x]) {
                    display.setPixel(x + position.x, y + position.y, true);
                }
            }
        }
        
        display.update();
    }
}