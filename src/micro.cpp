#include "micro.h"

// Constructor implementation
Micro::Micro() : peakToPeak(0), intensity(99), historyIndex(0) {
    // Initialize sample history
    for (int i = 0; i < HISTORY_SIZE; i++) {
        sampleHistory[i] = 0;
    }
}

// Destructor implementation
Micro::~Micro() {
    // Nothing to clean up
}

// Initialize the microphone input
void Micro::begin() {
    pinMode(MICRO_PIN, INPUT);
}

// Set the maximum intensity for the display
void Micro::setIntensity(uint8_t value) {
    intensity = value;
}

// Measure the amplitude of the microphone signal
void Micro::measureSignal() {
    unsigned long startMillis = millis();
    uint16_t signalMax = 0;
    uint16_t signalMin = 4095;

    // Sample the signal over the defined window
    while (millis() - startMillis < SAMPLE_WINDOW) {
        uint16_t sample = analogRead(MICRO_PIN);
        if (sample > signalMax) signalMax = sample;
        if (sample < signalMin) signalMin = sample;
    }

    peakToPeak = signalMax - signalMin;

    // Update the sample history
    sampleHistory[historyIndex] = peakToPeak;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
}