#include "freeFX1.h"

FreeFX1::FreeFX1(Settings* settings) {
    this->settings = settings;
}

void FreeFX1::begin() {
    x = random(0, MATRIX_WIDTH - 3); // Random initial position
    y = random(0, MATRIX_HEIGHT - 3);
    lastDirection = random(4); // Random initial direction
    lastMove = millis();
}

void FreeFX1::moveSquare() {
    int16_t newX = x;
    int16_t newY = y;
    
    // Choose new direction, avoiding opposite of last direction
    uint8_t newDirection;
    do {
        newDirection = random(4);
    } while ((newDirection + 2) % 4 == lastDirection);
    
    // Apply movement based on new direction
    switch(newDirection) {
        case 0: newY--; break; // Up
        case 1: newY++; break; // Down
        case 2: newX--; break; // Left
        case 3: newX++; break; // Right
    }
    
    // Update position if within bounds
    if (newX >= 0 && newX <= MATRIX_WIDTH - 3) {
        x = newX;
        lastDirection = newDirection;
    }
    if (newY >= 0 && newY <= MATRIX_HEIGHT - 3) {
        y = newY;
        lastDirection = newDirection;
    }
}

unsigned long FreeFX1::getDelay() {
    // Propotional delay calculation between 50ms and 1000ms
    uint8_t speed = settings->getFxSpeed();
    return map(speed, 10, 99, 1000, 50);
}

void FreeFX1::update(Display &display) {
    unsigned long currentTime = millis();
    
    // Verify if it is time to move the square
    if (currentTime - lastMove >= getDelay()) {
        lastMove = currentTime;
        moveSquare();
        
        // Clear screen
        display.clear();
        
        // Draw a 3x3 square to the current position
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                display.setPixel(x + i, y + j, true);
            }
        }
        
        // Update display
        display.update();
    }
}