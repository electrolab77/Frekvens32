#ifndef MICRO_FX4_H
#define MICRO_FX4_H

#include "micro.h"

class MicroFX4 : public Micro {
private:
    float currentScale;
    float targetScale;
    float scaleVelocity;
    
    void drawCircle(Display& matrix, float scale);
    void updateHeartScale();

public:
    MicroFX4();
    void begin() override;
    void update(Display& matrix) override;
};

#endif