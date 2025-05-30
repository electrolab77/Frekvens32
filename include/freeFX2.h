#ifndef FREE_FX2_H
#define FREE_FX2_H

#include "display.h"
#include "settings.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define MAX_BRANCHES 16
#define MATRIX_CENTER 8

struct Branch {
    int16_t x;
    int16_t y;
    uint8_t direction;
    bool active;
};

class FreeFX2 {
private:
    Settings* settings;
    Branch branches[MAX_BRANCHES];
    unsigned long lastGrowth = 0;
    uint8_t activeBranches = 1;

    void growBranches();
    unsigned long getDelay();
    bool canGrow(int16_t x, int16_t y);
    void startNewBranch();

public:
    FreeFX2(Settings* settings);
    void begin();
    void update(Display &display);
};

#endif