#include "freeFX3.h"

FreeFX3::FreeFX3(Settings* settings) {
    this->settings = settings;
    stagnationCounter = 0;
    wasActive = false;
    // Initialize all grids to false
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            grid[x][y] = false;
            nextGrid[x][y] = false;
            lastGrid[x][y] = false;
        }
    }
}

void FreeFX3::begin() {
    randomizeGrid();
    lastUpdate = millis();
    stagnationCounter = 0;
    // Initialize lastGrid different from grid to avoid immediate stagnation
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            lastGrid[x][y] = !grid[x][y];
        }
    }
}

void FreeFX3::randomizeGrid() {
    // Initialize with random cells (about 25% alive)
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            grid[x][y] = (random(100) < 25);
            lastGrid[x][y] = grid[x][y];
        }
    }
}

bool FreeFX3::hasLife() {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            if(grid[x][y]) return true;
        }
    }
    return false;
}

int FreeFX3::countNeighbors(int x, int y) {
    int count = 0;
    
    // Check all 8 neighboring cells
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            if(i == 0 && j == 0) continue; // Skip self
            
            // Handle wrap-around edges
            int checkX = (x + i + MATRIX_WIDTH) % MATRIX_WIDTH;
            int checkY = (y + j + MATRIX_HEIGHT) % MATRIX_HEIGHT;
            
            if(grid[checkX][checkY]) count++;
        }
    }
    
    return count;
}

void FreeFX3::computeNextGeneration() {
    // Apply Game of Life rules first
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            int neighbors = countNeighbors(x, y);
            if(grid[x][y]) {
                // Cell is alive
                nextGrid[x][y] = (neighbors == 2 || neighbors == 3);
            } else {
                // Cell is dead
                nextGrid[x][y] = (neighbors == 3);
            }
        }
    }
    
    // Then update grids
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            lastGrid[x][y] = grid[x][y];
            grid[x][y] = nextGrid[x][y];
        }
    }
}

bool FreeFX3::isGridStagnant() {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            if(grid[x][y] != lastGrid[x][y]) return false;
        }
    }
    return true;
}

unsigned long FreeFX3::getDelay() {
    // Calculate delay based on FX speed (10-99)
    uint8_t speed = settings->getFxSpeed();
    return map(speed, 10, 99, 1000, 50);
}

static const uint8_t STAGNATION_LIMIT = 5; // Changed from 5 to 10

void FreeFX3::update(Display &display) {
    unsigned long currentTime = millis();
    
    // Check if we just came back to this effect
    if (!wasActive) {
        DEBUG_PRINTLN("> Game of Life new start");
        randomizeGrid();
        stagnationCounter = 0;
        wasActive = true;
    }
    
    if(currentTime - lastUpdate >= getDelay()) {
        lastUpdate = currentTime;
        
        // Compute next generation first
        computeNextGeneration();
        
        // Then check for stagnation
        if(!hasLife() || isGridStagnant()) {
            stagnationCounter++;
            if(stagnationCounter >= STAGNATION_LIMIT) {
                DEBUG_PRINTLN("> Game of Life reset");
                randomizeGrid();
                stagnationCounter = 0;
            }
        } else {
            stagnationCounter = 0;
        }
        
        // Update display
        display.clear();
        for(int x = 0; x < MATRIX_WIDTH; x++) {
            for(int y = 0; y < MATRIX_HEIGHT; y++) {
                if(grid[x][y]) {
                    display.setPixel(x, y, true);
                }
            }
        }
        display.update();
    }
}

void FreeFX3::setWasActive(bool active) {
    wasActive = active;
}