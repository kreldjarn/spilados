#include "Arduino.h"
#include "Debouncer.h"


Debouncer::Debouncer(unsigned long bufferInterval, uint8_t pin)
{
    lastTime = millis();
    currentState = digitalRead(pin);
    pin = pin;
    bufferInterval = bufferInterval;
}

int Debouncer::update()
{
    uint8_t newState = digitalRead(pin);
    if (currentState != newState )
    {
        if (millis() - lastTime >= bufferInterval)
        {
            lastTime = millis();
            currentState = newState;
            return 1;
        }
    }
    return 0;
}

int Debouncer::read()
{
    return (int)currentState;
}

void Debouncer::write(int newState)
{
    currentState = newState;
    digitalWrite(pin, currentState);
}


