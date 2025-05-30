#ifndef MICRO_FX3_H
#define MICRO_FX3_H

#include <Arduino.h>
#include "display.h"
#include "micro.h"

class MicroFX3 : public Micro {
  private:
    uint16_t peakToPeak;                         // Signal amplitude
    uint8_t intensity;                           // LED intensity (0-99)
    uint16_t sampleHistory[HISTORY_SIZE];        // Measurements history
    uint8_t historyIndex;
    
    void measureSignal();                        // Measure signal amplitude
    uint8_t calculateHeight();                   // Calculate VU-meter height
    
  public:
    MicroFX3();
    void update(Display& matrix) override;       // Update display
};

#endif