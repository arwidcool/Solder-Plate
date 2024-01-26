#include "__enums.h"
#include <Arduino.h>

class Button
{
public:
  // Constructor
  Button(ButtonKind kind, uint8_t pin);

  ButtonKind getKind();
  uint8_t getPin();
  ButtonState getState();
  ButtonState setState(ButtonState state);
  void loop();
  

  private :
    unsigned long lastStateChangeTime;
    ButtonKind kind;
    uint8_t pin;
    ButtonState state;
};