#ifndef FREE_H
#define FREE_H

#include <Arduino.h>
#include "display.h"

class Free {
    protected:
        unsigned long lastUpdate = 0;
        
    public:
        virtual void begin() = 0;
        virtual void update(Display &display) = 0;
};

class FreeFX1 : public Free {
    public:
        void begin() override;
        void update(Display &display) override;
};

class FreeFX2 : public Free {
    public:
        void begin() override;
        void update(Display &display) override;
};

class FreeFX3 : public Free {
    public:
        void begin() override;
        void update(Display &display) override;
};

#endif