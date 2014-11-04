/***********************************
* Arduino code for 'Spilados'
*  written by
* Halldor Eldjarn (hae28@hi.is)
*  &
* Kristjan Eldjarn (keh4@hi.is)
************************************/

#include <Wire.h>
#include "SoftwareSerial.h"
#include "Adafruit_Trellis.h"

#define DEBUG false

//
// Constants
//
const int NUM_POTS = 8;
const int POT_PINS[NUM_POTS] = {8, 9, 10, 11, 12, 13, 14, 15};
const int SIGNIFICANT = 3;

//
// Store updated and old states of the pots
//
int pot_values[NUM_POTS];
int old_pot_values[NUM_POTS];

//
// Adafruit Trellis is the button array we are using
//
Adafruit_Trellis matrix = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix);

void setup()
{
  Serial.begin(9600);
  
  trellis.begin(0x70);
  
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  
  for(int i = 0; i < NUM_POTS; ++i)
  {
    old_pot_values[i] = 0;
    pot_values[i] = 0;
  }
}

// Use: Returns true if the distance between values is significant
boolean value_change_significant(int value, int new_value)
{
  if (abs(value - new_value) >= SIGNIFICANT)
  {
    return true;
  }
  return false;
}

void send_value(int index, int value)
{
#if DEBUG
  Serial.print(index);
  Serial.print(", ");
  Serial.println(value);
#else
  Serial.write(index);
  Serial.write(value);
#endif 
}

void loop()
{
  delay(30);
  
  // Handle pushing buttons and toggling
  // the LED on/off. 
  // TODO: Sync with internal state dumps of the
  // Sequencer over serial
  if(trellis.readSwitches())
  {
    for(int i = 0; i < 16; i++)
    {
      if(trellis.justPressed(i))
      {
        if(trellis.isLED(i))
        {
          send_value(100 + i, 0);
          trellis.clrLED(i);
        }
        else
        {
          send_value(100 + i, 255);
          trellis.setLED(i);
        }
      }
    }
  }
  
  trellis.writeDisplay();
  
  // Read the pots, send values over serial
  for(int i = 0; i < NUM_POTS; ++i)
  {
    int new_value = analogRead(POT_PINS[i]);
    
    if (value_change_significant(old_pot_values[i], new_value))
    {
      old_pot_values[i] = pot_values[i];
      pot_values[i] = new_value;
      send_value(i, new_value/4);
    }
  }
}