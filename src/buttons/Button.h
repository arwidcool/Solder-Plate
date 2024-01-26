#include "base.h"
#include <Arduino.h>
#include "../statechangeevent.h"

class Button
{
public:
  // Constructor
  Button(ButtonKind kind, uint8_t pin);

  ButtonKind getKind();
  uint8_t getPin();
  StateChangeEvent<ButtonKind, ButtonState> *lastChange();
  /// @brief  Call this method in the main loop to update the button state.
  /// @return true if the button state changed, false otherwise.
  bool loop();

private:
  ButtonKind kind;
  uint8_t pin;
  WrappedState<ButtonKind, ButtonState> state;
};