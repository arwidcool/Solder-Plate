#ifndef BUTTONS_H
#define BUTTONS_H

// Include any necessary libraries here
#include <Arduino.h>

enum ButtonState
{
  IDLE,
  PRESSED,
  RELEASED
};

enum Button
{
  UP,
  DOWN,
  BACK,
  SELECT,
  NONE
};



extern volatile bool upButtonPressed;
extern volatile bool downButtonPressed;
extern volatile bool backButtonPressed;
extern volatile bool selectButtonPressed;

extern enum ButtonState upButtonState;
extern enum ButtonState downButtonState;
extern enum ButtonState backButtonState;
extern enum ButtonState selectButtonState;



// LED pins
#define yellowLED 18
#define greenLED 19
#define redLED 20

// Button pins
#define upButton 21
#define downButton 22
#define backButton 23
#define selectButton 24
// Define any constants or macros here
#define BUTTON_PIN 2

// Declare any global variables here


// Declare any function prototypes here

class Buttons
{
public:
  // Constructor
  Buttons();
  ~Buttons();

  Button getPressedButton();
  Button handleButtons();

  void static upButtonISR();
  void static downButtonISR();
  void static backButtonISR();
  void static selectButtonISR();

  void static initializeButtons();

  // Public Variables
  bool pressed = false;
  bool released = false;
};

#endif // BUTTONS_H
