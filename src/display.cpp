#include "display.h"
#include "defines.h"

// Initialize the matrix
void Display::begin() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    
    // Configure PWM for brightness control
    ledcSetup(0, 5000, 8);
    ledcAttachPin(ENABLE_PIN, 0);
    setIntensity(10); // Default brightness
}

// Clear the display (full by default, top or botton)
void Display::clear(String where) {
    int min = 0;
    int max = 16;
    if (where == "top") {
        max = 8;
    } else if (where == "bottom") {
        min = 8;
    }
    for (int i = min; i < max; i++) {
        frame[i][0] = 0x00;
        frame[i][1] = 0x00;
    }
}

// Set the intensity of the display
void Display::setIntensity(uint8_t value) {
    // Convert 0-99 range to 0-255 range and invert for ENABLE pin
    ledcWrite(0, 255 - map(value, 0, 99, 0, 10)); // 255 replaced by 10
}

// Set a pixel on the display
void Display::setPixel(uint8_t x, uint8_t y, bool state) { // x = row, y = column -> TO REVIEW
    if (y >= MATRIX_WIDTH || x >= MATRIX_HEIGHT) return;
    
    if (state) {
        if (x < 8) {
            frame[y][0] |= (1 << (7 - x));
        } else {
            frame[y][1] |= (1 << (15 - x));
        }
    } else {
        if (y < 8) {
            frame[y][0] &= ~(1 << (7 - x));
        } else {
            frame[y][1] &= ~(1 << (15 - x));
        }
    }
}

void Display::drawChar(char c, int16_t x, int16_t y) { // x = row, y = column
    uint8_t pattern[Font6x6::CHAR_HEIGHT];
    Font6x6::getCharPattern(c, pattern);

    for (int row = 0; row < Font6x6::CHAR_HEIGHT; row++) {
        for (int col = 0; col < Font6x6::CHAR_WIDTH; col++) {
            if (x + row >= 0 && x + row < MATRIX_HEIGHT &&
                y + col >= 0 && y + col < MATRIX_WIDTH) {
                // Read bit from right to left (bit 5 = column 0)
                bool pixelOn = (pattern[row] >> (5 - col)) & 0x01;
                setPixel(y + col, x + row, pixelOn);
            }
        }
    }
}

// Display a static text
void Display::displayText(const String& text) {
    clear();
    int16_t x = (MATRIX_WIDTH - (text.length() * Font6x6::CHAR_WIDTH)) / 2;
    int16_t y = (MATRIX_HEIGHT - Font6x6::CHAR_HEIGHT) / 2;
    for (size_t i = 0; i < text.length(); i++) {
        drawChar(text[i], x + (i * Font6x6::CHAR_WIDTH), y);
    }
    update();
}

// Scroll text across the display
void Display::scrollText(const String& text, unsigned long delay, uint16_t passes) {
    currentScrollText = text;
    scrollPosition = MATRIX_WIDTH;
    scrollDelay = delay;
    lastScrollTime = millis();
    scrollPassCount = 0;
    scrollMaxPasses = passes;
}

void Display::updateScroll() {
    if (currentScrollText.length() == 0) return;

    // Managing timing with precision
    unsigned long now = millis();
    if (now - lastScrollTime < scrollDelay) return;
    lastScrollTime = now;

    clear("top");

    int16_t x = scrollPosition;
    for (size_t i = 0; i < currentScrollText.length(); i++) {
        char c = currentScrollText[i];
        drawChar(c, 1, x);
        if (c != ' ') {
            x += Font6x6::CHAR_WIDTH + 1;
        } else {
            x += Font6x6::CHAR_WIDTH;
        }
    }

    scrollPosition--;

    // Calculating the total length of the displayed text
    int totalWidth = 0;
    for (char c : currentScrollText) {
        totalWidth += Font6x6::CHAR_WIDTH;
        if (c != ' ') totalWidth += 1;
    }

    if (scrollPosition < -totalWidth) {
        scrollPosition = MATRIX_WIDTH;
        scrollPassCount++;
        
        // Stop if the maximum number of passes is reached
        if (scrollMaxPasses > 0 && scrollPassCount >= scrollMaxPasses) {
            currentScrollText = "";
            return;
        }
    }

    update();
}

// Display up to 4 char on the screen
void Display::displayValue(const String& value) {
    // If blinkg in progressn do not update the display
    if (!isBlinking) {
        // Initialisation of positions
        int16_t x1 = 1;
        int16_t x2 = 9;
        int16_t y1 = 1;
        int16_t y2 = 9;
        
        // Clear full or only bottom half of the display
        if (value.length() > 2) {
            clear();
        } else {
            clear("bottom");
        }
        
        // Affichage
        if (value.length() == 1) {
            drawChar(value[0], x2, y2);
        } else if (value.length() == 2) {
            drawChar(value[0], x2, y1);
            drawChar(value[1], x2, y2);
        } else {
            drawChar(value[0], x1, y1);
            drawChar(value[1], x1, y2);
            drawChar(value[2], x2, y1);
            drawChar(value[3], x2, y2);
        }
        update();
    }
}

void Display::blinkDisplay(unsigned long speed, uint8_t count) {
    DEBUG_PRINTLN("Starting blink display:");
    DEBUG_PRINTLN("  Speed: " + String(speed));
    DEBUG_PRINTLN("  Count: " + String(count));
    isBlinking = true;
    blinkState = true;
    blinkCount = 0;
    maxBlinks = count;
    blinkDelay = speed;
    lastBlinkTime = millis();
}

void Display::stopBlink() {
    isBlinking = false;
    blinkState = true;  // Ensure display is on when stopping
    update();  // Update display to show final state
}

void Display::updateBlink() {
    if (!isBlinking) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= blinkDelay) {
        lastBlinkTime = currentTime;
        blinkState = !blinkState;
        
        DEBUG_PRINTLN("Blink state: " + String(blinkState));
        
        if (!blinkState) {
            blinkCount++;
            DEBUG_PRINTLN("Blink count: " + String(blinkCount) + "/" + String(maxBlinks));
            
            if (maxBlinks > 0 && blinkCount >= maxBlinks) {
                DEBUG_PRINTLN("Blink complete");
                stopBlink();
                return;
            }
        }
        
        // Apply the current state
        if (blinkState) {
            sendFrame();  // Show current frame content
        } else {
            // Turn off display without modifying frame buffer
            digitalWrite(LATCH_PIN, LOW);
            for (int i = 0; i < 32; i++) {
                shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x00);
            }
            digitalWrite(LATCH_PIN, HIGH);
        }
    }
}

// Update the display
void Display::update() {
    sendFrame();
}

// Send frame to the SCT2024
void Display::sendFrame() {
    digitalWrite(LATCH_PIN, LOW);
    for (int i = 0; i < 16; i++) {
        shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, frame[i][0]);
    }
    for (int i = 0; i < 16; i++) {
        shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, frame[i][1]);
    }
    digitalWrite(LATCH_PIN, HIGH);
}