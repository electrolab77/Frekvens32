#ifndef PULSE_H
#define PULSE_H

#include <Arduino.h>

class Pulse {
  private:
    static const uint8_t PULSE_PIN = 35;
    static const int THRESHOLD = 2048;
    static const int SAMPLES = 4;
    static const int PERIOD_SAMPLES = 8;
    
    uint8_t ppqn;
    bool lastState;
    unsigned long lastTime;
    unsigned long period;
    float bpm;
    bool pulseDetected;
    
    int samples[SAMPLES];
    uint8_t sampleIndex;
    
    unsigned long periodSamples[PERIOD_SAMPLES];
    uint8_t periodIndex;
    
    bool readInput();
    void updatePeriod(unsigned long newPeriod);
    
  public:
    Pulse();
    void begin();
    void setPPQN(uint8_t value);
    uint8_t getPPQN();
    float getBPM();
    unsigned long getPeriod();
    bool update();
    bool isPulseDetected();
    void reset();
    bool isActive();
};

#endif