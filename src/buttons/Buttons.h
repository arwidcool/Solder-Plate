#ifndef BUTTONS_H
#define BUTTONS_H

// Include any necessary libraries here
#include <Arduino.h>
#include "Button.h"
#include "leds/leds.h"
#include "../common.h"
// Button pins
#define upButton 21
#define downButton 22
#define backButton 23
#define selectButton 24

class Buttons
{
public:
  // Constructor
  Buttons(){};

  Pair<ButtonKind, StateChangeEvent<ButtonState>> *handleButtons();
  void setup();
};
#define ISBUTTONMIGRATEDTOSTATE(pair, kind, state) (((pair).first == kind) && ((pair).second.to == state))
#endif // BUTTONS_H
