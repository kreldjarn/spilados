const int NUM_POTS = 2;
const int POT_PINS[NUM_POTS] = {0, 1};
const int SIGNIFICANT = 3;

// Pot values are stored as [0; 1023]
int pot_values[NUM_POTS];
int old_pot_values[NUM_POTS];

int button_state = 0;
int old_button_state = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  
  for(int i = 0; i < NUM_POTS; ++i)
  {
    old_pot_values[i] = 0;
    pot_values[i] = 0;
  }
}

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
  Serial.write(index);
  Serial.write(value);
  Serial.write(0);
}

void loop()
{
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
  
  old_button_state = button_state;
  button_state = digitalRead(2);
  
  if(old_button_state > button_state)
  {
    send_value(2, 255);
    //Serial.println("Button pushed down!");
  }  
  else if(old_button_state < button_state)
  {
    send_value(2, 0);
    //Serial.println("Button has returned to his natural state of equilibrium!");
  }    
}