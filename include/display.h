#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "font6x6.h"

class Display {
  private:
    // Pin definitions
    static const uint8_t DATA_PIN   = 23;
    static const uint8_t CLOCK_PIN  = 18;
    static const uint8_t LATCH_PIN  = 5;
    static const uint8_t ENABLE_PIN = 4;

    // Matrix dimensions
    static const uint8_t MATRIX_WIDTH  = 16;
    static const uint8_t MATRIX_HEIGHT = 16;

    // Display buffer using the same format as the working example
    uint8_t frame[16][2];

    // Scrolling text parameters
    int16_t scrollPosition;
    String currentScrollText;
    unsigned long lastScrollTime;
    unsigned long scrollDelay;

    // Send the current frame to the display
    void sendFrame();

  public:
    Display();
    void begin();
    void clear(String where = "full");
    void testChar();
    void setIntensity(uint8_t value);
    void setPixel(uint8_t x, uint8_t y, bool state); // x = row, y = column
    void drawChar(char c, int16_t x, int16_t y); // x = row, y = column
    void displayText(const String& text);
    void scrollText(const String& text, unsigned long delay);
    void updateScroll();
    void displayValue(const String& value);
    void update();
};

#endif