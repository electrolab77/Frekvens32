#include "freeFX2.h"

FreeFX2::FreeFX2(Settings* settings) {
    this->settings = settings;
}

void FreeFX2::begin() {
    // Start with one branch from center
    branches[0] = {MATRIX_CENTER, MATRIX_CENTER, static_cast<uint8_t>(random(4)), true};
    activeBranches = 1;
    
    // Initialize other branches as inactive
    for(int i = 1; i < MAX_BRANCHES; i++) {
        branches[i].active = false;
    }
    
    lastGrowth = millis();
}

bool FreeFX2::canGrow(int16_t x, int16_t y) {
    // Check matrix bounds
    if(x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) {
        return false;
    }
    
    // Check surrounding pixels to avoid crossing
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            if(i == 0 && j == 0) continue;
            int16_t checkX = x + i;
            int16_t checkY = y + j;
            if(checkX >= 0 && checkX < MATRIX_WIDTH && 
               checkY >= 0 && checkY < MATRIX_HEIGHT) {
                // If pixel is already lit, don't grow here
                // We'll need to check the actual display state
            }
        }
    }
    return true;
}

void FreeFX2::growBranches() {
    for(int i = 0; i < MAX_BRANCHES; i++) {
        if(!branches[i].active) continue;
        
        int16_t newX = branches[i].x;
        int16_t newY = branches[i].y;
        
        // 70% chance to continue in same direction
        if(random(100) < 70) {
            switch(branches[i].direction) {
                case 0: newY--; break; // Up
                case 1: newX++; break; // Right
                case 2: newY++; break; // Down
                case 3: newX--; break; // Left
            }
        } else {
            // Change direction slightly (45 degrees)
            branches[i].direction = (branches[i].direction + (random(2) ? 1 : 3)) % 4;
            continue;
        }
        
        if(canGrow(newX, newY)) {
            branches[i].x = newX;
            branches[i].y = newY;
            
            // 20% chance to spawn new branch
            if(random(100) < 20 && activeBranches < MAX_BRANCHES) {
                startNewBranch();
            }
        } else {
            // Branch is blocked, make it inactive
            branches[i].active = false;
        }
    }
}

void FreeFX2::startNewBranch() {
    for(int i = 0; i < MAX_BRANCHES; i++) {
        if(!branches[i].active) {
            // Find random active branch to split from
            int sourceBranch = random(activeBranches);
            branches[i] = {
                branches[sourceBranch].x,
                branches[sourceBranch].y,
                static_cast<uint8_t>((branches[sourceBranch].direction + (random(2) ? 1 : 3)) % 4),
                true
            };
            activeBranches++;
            break;
        }
    }
}

unsigned long FreeFX2::getDelay() {
    uint8_t speed = settings->getFxSpeed();
    return map(speed, 10, 99, 1000, 50);
}

void FreeFX2::update(Display &display) {
    unsigned long currentTime = millis();
    
    if(currentTime - lastGrowth >= getDelay()) {
        lastGrowth = currentTime;
        
        // Check if any branches are still active
        bool anyActive = false;
        for(int i = 0; i < MAX_BRANCHES; i++) {
            if(branches[i].active) {
                anyActive = true;
                break;
            }
        }
        
        // If no active branches, restart
        if(!anyActive) {
            begin();
        }
        
        growBranches();
        
        // Draw all branch positions
        display.clear();
        for(int i = 0; i < MAX_BRANCHES; i++) {
            if(branches[i].active) {
                display.setPixel(branches[i].x, branches[i].y, true);
            }
        }
        display.update();
    }
}