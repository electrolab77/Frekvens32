#include "microFX2.h"

MicroFX2::MicroFX2() : Micro() {
    initParticles();
}

void MicroFX2::initParticles() {
    for(int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].x = random(16);
        particles[i].y = random(16);
        particles[i].vx = random(-100, 100) / 100.0;
        particles[i].vy = random(-100, 100) / 100.0;
    }
}

void MicroFX2::updateParticles(float energy) {
    for(int i = 0; i < NUM_PARTICLES; i++) {
        // Update positions
        particles[i].x += particles[i].vx * energy;
        particles[i].y += particles[i].vy * energy;
        
        // Bounce on edges
        if(particles[i].x < 0) {
            particles[i].x = 0;
            particles[i].vx *= -1;
        }
        if(particles[i].x >= 15) {
            particles[i].x = 15;
            particles[i].vx *= -1;
        }
        if(particles[i].y < 0) {
            particles[i].y = 0;
            particles[i].vy *= -1;
        }
        if(particles[i].y >= 15) {
            particles[i].y = 15;
            particles[i].vy *= -1;
        }
    }
}

void MicroFX2::update(Display& matrix) {
    measureSignal();
    
    // Calculate energy (0.0 - 1.0)
    float energy = constrain(map(peakToPeak, 0, 2200, 0, 100), 0, 100) / 100.0;
    
    // Update particles
    updateParticles(energy);
    
    // Clear matrix
    matrix.clear();
    
    // Draw particles
    for(int i = 0; i < NUM_PARTICLES; i++) {
        if(random(100) < intensity) {
            matrix.setPixel(particles[i].x, particles[i].y, true);
        }
    }
    
    matrix.update();
}