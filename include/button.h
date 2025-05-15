#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

// Defines for pins
#define PIN_BTN_YELLOW  17 // GPIO17 for yellow button
#define PIN_BTN_RED     16 // GPIO16 for red button

class Button {
  private:
    uint8_t pin;                     // Button pin
    bool lastState;                  // Last read state
    bool currentState;               // Current state
    unsigned long lastDebounceTime;  // Last debounce time
    unsigned long pressStartTime;    // Time when the button was pressed
    bool isPressed;                  // Current pressed state
    bool wasLongPress;               // Flag to avoid multiple long press detections

    // Timing constants
    static const unsigned long DEBOUNCE_DELAY = 50;      // Debounce delay in ms
    static const unsigned long LONG_PRESS_TIME = 1000;   // Long press duration in ms

    // Function pointers for callbacks
    void (*shortPressCallback)();
    void (*longPressCallback)();

  public:
    // Constructor
    Button(uint8_t buttonPin);

    // Initialize the button
    void begin();

    // Set the short press callback
    void setShortPressCallback(void (*callback)());

    // Set the long press callback
    void setLongPressCallback(void (*callback)());

    // Update the button state
    void update();
};

#endif