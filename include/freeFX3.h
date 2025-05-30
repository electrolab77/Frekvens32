#ifndef FREE_FX3_H
#define FREE_FX3_H

#include "display.h"
#include "settings.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

class FreeFX3 {
private:
    Settings* settings;
    bool grid[MATRIX_WIDTH][MATRIX_HEIGHT];
    bool nextGrid[MATRIX_WIDTH][MATRIX_HEIGHT];
    unsigned long lastUpdate = 0;

    void randomizeGrid();
    int countNeighbors(int x, int y);
    void computeNextGeneration();
    unsigned long getDelay();
    
public:
    FreeFX3(Settings* settings);
    void begin();
    void update(Display &display);
};

#endif