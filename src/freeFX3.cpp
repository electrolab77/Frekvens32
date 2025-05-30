#include "freeFX3.h"

FreeFX3::FreeFX3(Settings* settings) {
    this->settings = settings;
}

void FreeFX3::begin() {
    randomizeGrid();
    lastUpdate = millis();
}

void FreeFX3::randomizeGrid() {
    // Initialize with random cells (about 40% alive)
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            grid[x][y] = (random(100) < 40);
        }
    }
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
    // Apply Conway's Game of Life rules:
    // 1. Any live cell with fewer than two live neighbors dies (underpopulation)
    // 2. Any live cell with two or three live neighbors lives on
    // 3. Any live cell with more than three live neighbors dies (overpopulation)
    // 4. Any dead cell with exactly three live neighbors becomes alive (reproduction)
    
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
    
    // Copy next generation to current
    for(int x = 0; x < MATRIX_WIDTH; x++) {
        for(int y = 0; y < MATRIX_HEIGHT; y++) {
            grid[x][y] = nextGrid[x][y];
        }
    }
}

unsigned long FreeFX3::getDelay() {
    uint8_t speed = settings->getFxSpeed();
    return map(speed, 10, 99, 1000, 50);
}

void FreeFX3::update(Display &display) {
    unsigned long currentTime = millis();
    
    if(currentTime - lastUpdate >= getDelay()) {
        lastUpdate = currentTime;
        
        // Check if grid is static or empty
        bool hasLife = false;
        for(int x = 0; x < MATRIX_WIDTH && !hasLife; x++) {
            for(int y = 0; y < MATRIX_HEIGHT && !hasLife; y++) {
                if(grid[x][y]) hasLife = true;
            }
        }
        
        if(!hasLife) {
            randomizeGrid(); // Restart if no cells are alive
        } else {
            computeNextGeneration();
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