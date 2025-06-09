#include "microFX4.h"
#include "defines.h"
#include <math.h>

MicroFX4::MicroFX4() : currentScale(0.0f), targetScale(0.0f), scaleVelocity(0.0f) {
}

void MicroFX4::begin() {
    Micro::begin();
    currentScale = 0.0f;
    targetScale = 0.0f;
    scaleVelocity = 0.0f;
}

void MicroFX4::update(Display& matrix) {
    // Measure sound
    measureSignal();
    
    // Map peak-to-peak to target scale with more aggressive range
    float newTarget = constrain(map(peakToPeak, 0, 2000, 0, 100) / 100.0f, 0.0f, 1.0f);
    
    // Much faster transition
    targetScale = targetScale * 0.3f + newTarget * 0.7f;  // More weight on new value
    
    // Update heart scale
    updateHeartScale();
    
    // Draw heart
    matrix.clear();
    drawCircle(matrix, currentScale);
    matrix.update();
    
    //DEBUG_PRINTLN("Sound: " + String(peakToPeak) + " Target: " + String(newTarget) + " Scale: " + String(currentScale));
}

void MicroFX4::updateHeartScale() {
    // More aggressive physics
    float springForce = (targetScale - currentScale) * 0.8f;  // Increased spring force
    float damping = -scaleVelocity * 0.2f;  // Reduced damping
    
    scaleVelocity += springForce + damping;
    
    // Add faster oscillation when no sound
    if (targetScale < 0.1f) {
        scaleVelocity += sin(millis() / 300.0f) * 0.03f;  // Faster and stronger pulse
    }
    
    currentScale += scaleVelocity;
    
    // Ensure full range of motion
    currentScale = constrain(currentScale, 0.0f, 1.0f);
}

void MicroFX4::drawCircle(Display& matrix, float scale) {
    // Scale circle size from 4 to 8 radius
    float radius = 4.0f + (scale * 4.0f);
    
    // Center position on 16x16 matrix
    const float centerX = 8.0f;
    const float centerY = 8.0f;
    
    // Draw circle using mathematical formula
    for(int y = 0; y < 16; y++) {
        for(int x = 0; x < 16; x++) {
            // Calculate distance from center
            float dx = x - centerX;
            float dy = y - centerY;
            float distance = sqrt(dx * dx + dy * dy);
            
            // Draw pixel if within radius (with anti-aliasing)
            if (distance <= radius) {
                matrix.setPixel(x, y, true);
            }
        }
    }
    
    //DEBUG_PRINTLN("Circle radius: " + String(radius));
}