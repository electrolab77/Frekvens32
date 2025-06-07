#ifndef FREE_FX3_H
#define FREE_FX3_H

#include "display.h"
#include "settings.h"
#include "defines.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

class FreeFX3 {
private:
    Settings* settings;
    bool grid[MATRIX_WIDTH][MATRIX_HEIGHT];
    bool nextGrid[MATRIX_WIDTH][MATRIX_HEIGHT];
    bool lastGrid[MATRIX_WIDTH][MATRIX_HEIGHT];
    unsigned long lastUpdate;
    uint8_t stagnationCounter;
    static const uint8_t STAGNATION_LIMIT = 5;
    static const uint8_t HISTORY_SIZE = 4;  // Nombre de générations à mémoriser
    bool historyGrid[HISTORY_SIZE][MATRIX_WIDTH][MATRIX_HEIGHT];
    uint8_t historyIndex;

    void randomizeGrid();
    int countNeighbors(int x, int y);
    void computeNextGeneration();
    bool isGridStagnant();
    unsigned long getDelay();
    bool hasLife();
    bool wasActive;
    bool isOscillating();

public:
    FreeFX3(Settings* settings);
    void begin();
    void update(Display &display);
    void setWasActive(bool active); 
};

#endif