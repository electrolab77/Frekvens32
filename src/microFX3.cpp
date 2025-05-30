#include "microFX3.h"

MicroFX3::MicroFX3() : peakToPeak(0), intensity(50), historyIndex(0) {
    // Initialize history
    for(int i = 0; i < HISTORY_SIZE; i++) {
        sampleHistory[i] = 0;
    }
}

void MicroFX3::measureSignal() {
    unsigned long startMillis = millis();
    uint16_t signalMax = 0;
    uint16_t signalMin = 4095;
    
    // Sample over time window
    while (millis() - startMillis < SAMPLE_WINDOW) {
        uint16_t sample = analogRead(MICRO_PIN);
        if (sample > signalMax) signalMax = sample;
        if (sample < signalMin) signalMin = sample;
    }
    
    peakToPeak = signalMax - signalMin;
    
    // Update history
    sampleHistory[historyIndex] = peakToPeak;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
}

uint8_t MicroFX3::calculateHeight() {
    // Moving average over history
    uint32_t sum = 0;
    for(int i = 0; i < HISTORY_SIZE; i++) {
        sum += sampleHistory[i];
    }
    uint16_t average = sum / HISTORY_SIZE;
    
    // Convert to height (0-16)
    return map(average, 0, 2500, 0, 16);
}

void MicroFX3::update(Display& matrix) {
    measureSignal();
    uint8_t height = calculateHeight();
    
    // Clear matrix
    matrix.clear();
    
    // Draw VU-meter
    for(int i = 0; i < height; i++) {
        for(int x = 0; x < 16; x++) {
            // Calculate variable intensity based on height
            uint8_t rowIntensity = map(i, 0, 15, intensity, intensity/4);
            if(random(100) < rowIntensity) {
                matrix.setPixel(x, 15-i, true);
            }
        }
    }
    
    // Update display
    matrix.update();
}