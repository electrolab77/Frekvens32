#ifndef MICRO_H
#define MICRO_H

#include <Arduino.h>
#include "display.h"

// Base class for microphone effects
class Micro {
  protected:
    static const uint8_t  MICRO_PIN     = 35;  // Analog pin for the microphone
    static const uint16_t SAMPLE_WINDOW = 50;  // Sampling window in milliseconds
    static const uint16_t HISTORY_SIZE  = 16;  // Size of the signal history buffer

    uint16_t peakToPeak;                       // Amplitude of the signal
    uint8_t intensity;                         // Maximum intensity for display
    uint16_t sampleHistory[HISTORY_SIZE];      // History of signal amplitudes
    uint8_t historyIndex;                      // Current index in history buffer

    virtual void measureSignal();              // Measure signal amplitude

  public:
    Micro();                                   // Constructor
    virtual ~Micro();                          // Virtual destructor
    virtual void begin();                      // Initialize microphone
    virtual void setIntensity(uint8_t value); // Set display intensity
    virtual void update(Display& matrix) = 0;  // Update display (pure virtual)
};

#endif