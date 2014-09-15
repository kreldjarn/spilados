#ifndef Debouncer_h
#define Debouncer_h

#include <inttypes.h>

class Debouncer
{
    public:
        Debouncer(unsigned long bufferInterval, uint8_t pin);
        int update();
        int read();
        void write(int newState);
      
    private:
        unsigned long  lastTime, bufferInterval;
        uint8_t currentState;
        uint8_t pin;
};

#endif


