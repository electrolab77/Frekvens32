#include "pulse.h"

Pulse::Pulse() 
    : ppqn(24), lastState(false), lastTime(0), 
      period(0), bpm(0), pulseDetected(false),
      sampleIndex(0), periodIndex(0) {
    for(int i = 0; i < SAMPLES; i++) samples[i] = 0;
    for(int i = 0; i < PERIOD_SAMPLES; i++) periodSamples[i] = 0;
}

void Pulse::begin() {
    pinMode(PULSE_PIN, INPUT);
}

void Pulse::setPPQN(uint8_t value) {
    ppqn = value;
}

uint8_t Pulse::getPPQN() {
    return ppqn;
}

float Pulse::getBPM() {
    return bpm;
}

unsigned long Pulse::getPeriod() {
    return period;
}

bool Pulse::readInput() {
    samples[sampleIndex] = analogRead(PULSE_PIN);
    sampleIndex = (sampleIndex + 1) % SAMPLES;
    
    int average = 0;
    for(int i = 0; i < SAMPLES; i++) {
        average += samples[i];
    }
    average /= SAMPLES;
    
    return average > THRESHOLD;
}

void Pulse::updatePeriod(unsigned long newPeriod) {
    periodSamples[periodIndex] = newPeriod;
    periodIndex = (periodIndex + 1) % PERIOD_SAMPLES;
    
    unsigned long sum = 0;
    for(int i = 0; i < PERIOD_SAMPLES; i++) {
        sum += periodSamples[i];
    }
    period = sum / PERIOD_SAMPLES;
    
    if(period > 0) {
        bpm = 60000.0 / (float)(period * ppqn);
    }
}

bool Pulse::update() {
    pulseDetected = false;
    bool currentState = readInput();
    
    if(currentState && !lastState) {
        unsigned long currentTime = millis();
        if(lastTime > 0) {
            updatePeriod(currentTime - lastTime);
        }
        lastTime = currentTime;
        pulseDetected = true;
    }
    
    lastState = currentState;
    return pulseDetected;
}

bool Pulse::isPulseDetected() {
    return pulseDetected;
}

void Pulse::reset() {
    lastTime = 0;
    period = 0;
    bpm = 0;
    for(int i = 0; i < PERIOD_SAMPLES; i++) {
        periodSamples[i] = 0;
    }
}

bool Pulse::isActive() {
    return (millis() - lastTime) < (period * 2);
}