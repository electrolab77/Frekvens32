#ifndef MICRO_FX2_H
#define MICRO_FX2_H

#include "micro.h"

class MicroFX2 : public Micro {
  private:
    static const uint8_t NUM_PARTICLES = 10;  // Number of particles
    
    struct Particle {
      float x;
      float y;
      float vx;
      float vy;
    };
    
    Particle particles[NUM_PARTICLES];
    
    // Initialize particles
    void initParticles();
    
    // Update particle positions
    void updateParticles(float energy);
    
  public:
    MicroFX2();
    void update(Display& matrix) override;
};

#endif