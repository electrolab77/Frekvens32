#ifndef FONT6X6_H
#define FONT6X6_H

#include <Arduino.h>

// 6x6 pixel font (characters 0-9, A-Z and some symbols)
extern const uint8_t FONT_6X6[] PROGMEM;

// Class to manage the font
class Font6x6 {
  public:
    static const uint8_t CHAR_WIDTH = 6;
    static const uint8_t CHAR_HEIGHT = 6;

    // Get the pattern for a character
    static void getCharPattern(char c, uint8_t pattern[CHAR_HEIGHT]);
};

#endif