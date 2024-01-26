#ifndef BUTTONS_H
#define BUTTONS_H

// Include any necessary libraries here
#include <Arduino.h>
#include "Button.h"
#include "leds/leds.h"

// Button pins
#define upButton 21
#define downButton 22
#define backButton 23
#define selectButton 24


class Buttons
{
public:
  // Constructor
  Buttons();

  ButtonKind handleButtons();
  ButtonKind getPressedButton();
  void static initializeButtons();
  Button** getAllButtons();


  void handleButtonLeds();

};

#endif // BUTTONS_H
