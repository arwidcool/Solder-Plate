#include "base.h"
#include <Arduino.h>

class Button
{
public:
  // Constructor
  Button(ButtonKind kind, uint8_t pin);

  ButtonKind getKind();
  uint8_t getPin();
  ButtonState getState();
  ButtonStateChange* lastChange();
  void setState(ButtonState state);
  /// @brief  Call this method in the main loop to update the button state.
  /// @return true if the button state changed, false otherwise.
  bool loop();
  

  private :
    unsigned long lastStateChangeTime;
    ButtonKind kind;
    uint8_t pin;
    ButtonState state;
    ButtonStateChange* change;
};