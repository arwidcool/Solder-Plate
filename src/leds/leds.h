#ifndef __leds_h__
#define __leds_h__
#include "../buttons/base.h"
#include "../common.h"
#include "../globals.h"

// If you didnt solder the LEDS in order, change the order here just change the pin numbers till it matches your board
#define LEFT_LED_PIN 20 //Should be RED
#define MID_LED_PIN 18 //Should be YELLOW
#define RIGHT_LED_PIN 19 //Should be GREEN
#define GREEN_LED_PIN RIGHT_LED_PIN


enum LedDesiredState
{
  LED_ON,
  LED_OFF,
  LED_BLINKING
};

class LEDS
{
public:
  LEDS(){};
  void setup()
  {
    pinMode(LEFT_LED_PIN, OUTPUT);
    pinMode(MID_LED_PIN, OUTPUT);
    pinMode(RIGHT_LED_PIN, OUTPUT);
  }
  void loop()
  {
    ReflowProcessState state = reflowProcessState.get();
    if (state >= PREHEAT && state < DONE)
    {
      leftLED = (state == PREHEAT || state == COOL) ? LED_BLINKING : LED_ON;
      midLED = (state == SOAK || state == COOL) ? LED_BLINKING : (state == PREHEAT ? LED_OFF : LED_ON);
      rightLED = (state == REFLOW || state == COOL) ? LED_BLINKING : ((state == PREHEAT || state == SOAK) ? LED_OFF : LED_ON);

    }
    else if (state == DONE)
    {
      leftLED = midLED = rightLED = LED_OFF;
    } 
    setLED(LEFT_LED_PIN, leftLED);
    setLED(MID_LED_PIN, midLED);
    setLED(RIGHT_LED_PIN, rightLED);
  }

  void setLED(int pin, LedDesiredState state)
  {
    bool goingToBeOn = state == LED_ON || (state == LED_BLINKING && millis() % 1000 < 500);
    if (goingToBeOn && pin == GREEN_LED_PIN)
    {
      analogWrite(pin, 30);
    }
    else
    {
      digitalWrite(pin, goingToBeOn ? HIGH : LOW);
    }
  }
  /**
   * @brief Handles the state change of a button
   * CALL only if the global state is USER_INPUT
   */
  void handleButtonStateChange(Pair<ButtonKind, StateChangeEvent<ButtonState>> change)
  {
    switch (change.first)
    {
    case ButtonKind::UP:
    case ButtonKind::DOWN:
      midLED = (change.second.to == ButtonState::PRESSED) ? LED_ON : LED_OFF;      
      break;
    case ButtonKind::BACK:
      leftLED = (change.second.to == ButtonState::PRESSED) ? LED_ON : LED_OFF;
      break;
    case ButtonKind::SELECT:
      rightLED = (change.second.to == ButtonState::PRESSED) ? LED_ON : LED_OFF;
      break;
    default:
      break;
    }
  }

private:
  LedDesiredState leftLED = LedDesiredState::LED_OFF;
  LedDesiredState midLED = LedDesiredState::LED_OFF;
  LedDesiredState rightLED = LedDesiredState::LED_OFF;
};
#endif