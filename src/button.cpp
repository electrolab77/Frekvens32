#include "button.h"

// Constructor
Button::Button(uint8_t buttonPin) {
    pin                = buttonPin;
    lastState          = HIGH;
    currentState       = HIGH;
    lastDebounceTime   = 0;
    pressStartTime     = 0;
    isPressed          = false;
    wasLongPress       = false;
    shortPressCallback = nullptr;
    longPressCallback  = nullptr;
}

// Initialize the button
void Button::begin() {
    pinMode(pin, INPUT_PULLUP);
}

// Set the short press callback
void Button::setShortPressCallback(void (*callback)()) {
    shortPressCallback = callback;
}

// Set the long press callback
void Button::setLongPressCallback(void (*callback)()) {
    longPressCallback = callback;
}

// Update the button state
void Button::update() {
    // Read the current state
    bool reading = digitalRead(pin);

    // If the state has changed, reset the debounce timer
    if (reading != lastState) {
        lastDebounceTime = millis();
    }

    // If enough time has passed since the last state change
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        // If the state has changed
        if (reading != currentState) {
            currentState = reading;

            // If the button was just pressed
            if (currentState == LOW) {
                pressStartTime = millis();
                isPressed = true;
                wasLongPress = false;
            }
            // If the button was just released
            else if (isPressed) {
                isPressed = false;
                // If it wasn't a long press, it's a short press
                if (!wasLongPress && shortPressCallback != nullptr) {
                    shortPressCallback();
                }
            }
        }
    }

    // Detect long press
    if (isPressed && !wasLongPress) {
        if ((millis() - pressStartTime) > LONG_PRESS_TIME) {
            wasLongPress = true;
            if (longPressCallback != nullptr) {
                longPressCallback();
            }
        }
    }

    lastState = reading;
}