#ifndef MICRO_FX3_H
#define MICRO_FX3_H

#include <Arduino.h>
#include "display.h"
#include "micro.h"

class MicroFX3 : public Micro {
private:
    static const uint8_t NUM_BANDS = 16;         // Number of frequency bands
    static const uint16_t SAMPLES = 128;         // Number of samples for analysis
    static const uint8_t MAX_HEIGHT = 16;        // Maximum height of bands
    static const uint8_t DECAY_RATE = 2;         // Rate at which peaks fall
    
    uint8_t bandLevels[NUM_BANDS];              // Current level for each band
    uint8_t peakLevels[NUM_BANDS];              // Peak level for each band
    unsigned long lastDecayTime;                 // Last time peaks were decayed

    void analyzeMicrophoneData();               // Analyze microphone input
    void updatePeaks();                         // Update peak levels with decay

public:
    MicroFX3();
    void begin();
    void update(Display& matrix) override;
};

#endif