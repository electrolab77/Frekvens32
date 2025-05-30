#include "microFX1.h"

MicroFX1::MicroFX1() {
    for(int i = 0; i < NUM_BANDS; i++) {
        bandLevels[i] = 0;
        peakLevels[i] = 0;
    }
    lastDecayTime = 0;
}

void MicroFX1::begin() {
    pinMode(MICRO_PIN, INPUT);
}

void MicroFX1::analyzeMicrophoneData() {
    uint32_t sum = 0;
    uint16_t maxVal = 0;
    
    // Sample audio multiple times
    for(int i = 0; i < SAMPLES; i++) {
        uint16_t sample = analogRead(MICRO_PIN);
        sum += sample;
        if(sample > maxVal) maxVal = sample;
        delayMicroseconds(100);  // Small delay between samples
    }
    
    uint16_t avg = sum / SAMPLES;
    uint16_t amplitude = maxVal - avg;
    
    // Distribute amplitude across bands with some randomization
    for(int i = 0; i < NUM_BANDS; i++) {
        int value = map(amplitude, 0, 2048, 0, MAX_HEIGHT);
        value += random(-2, 3);  // Add some randomness
        bandLevels[i] = constrain(value, 0, MAX_HEIGHT);
    }
}

void MicroFX1::updatePeaks() {
    unsigned long currentTime = millis();
    
    // Update peaks and apply decay
    if(currentTime - lastDecayTime > 50) {  // Decay every 50ms
        lastDecayTime = currentTime;
        
        for(int i = 0; i < NUM_BANDS; i++) {
            // Update peak if current level is higher
            if(bandLevels[i] > peakLevels[i]) {
                peakLevels[i] = bandLevels[i];
            }
            // Otherwise decay peak
            else if(peakLevels[i] > 0) {
                peakLevels[i] = max(0, (int)peakLevels[i] - DECAY_RATE);
            }
        }
    }
}

void MicroFX1::update(Display& matrix) {
    analyzeMicrophoneData();
    updatePeaks();
    
    // Clear display
    matrix.clear();
    
    // Draw frequency bands and peaks
    for(int x = 0; x < NUM_BANDS; x++) {
        // Draw band level
        for(int y = 0; y < bandLevels[x]; y++) {
            matrix.setPixel(x, 15-y, true);
        }
        
        // Draw peak
        if(peakLevels[x] > 0) {
            matrix.setPixel(x, 15-peakLevels[x], true);
        }
    }
    
    // Update display
    matrix.update();
}