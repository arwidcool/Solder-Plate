#include <Arduino.h>
#include "ArduPID.h"
#include <Adafruit_GFX.h>
#include <voltageReference/AnalogRef.h>
#include <Wire.h>
#include <thermistors/Thermistor.h>
#include "buttons/Buttons.h"
#include "leds/leds.h"
#include "reflow.h"
#include "displays/oled.h"
#include "displays/tft.h"
#include "PID/PidController.h"
#include "globals.h"
#include "EEPROMDataManager.h"
#include "thermistors/TemperatureController.h"




#define MOSTFET_PIN 17

double currentTemp = 0;
double targetTemp = 60;
double pwmValue = 255;

Buttons buttons = Buttons();
LEDS leds = LEDS();
// Declare the PID
ArduPID PID;
OledDisplay oled = OledDisplay();

TFT_Display tftDisplay ;
TemperatureController temperatureController;





void setup()
{

  pinMode(MOSTFET_PIN, OUTPUT);
  analogWrite(MOSTFET_PIN, 255); // VERY IMPORTANT, DONT CHANGE!

  Serial.begin(38400);

  Serial.println("Starting OLED");
  // Set PWM frequency to 64 kHz
  analogWriteFrequency(64);

  buttons.setup();
  leds.setup();
  oled.setup();
  eepromDataManager.setup();

  reflowProcessState.set(ReflowProcessState::USER_INPUT);

  temperatureController.checkPluggedInThermistors();

  tftDisplay.start();
}
void loop()
{

  // Return the button that changed state
  Pair<ButtonKind, StateChangeEvent<ButtonState>> *k = buttons.handleButtons();
  ReflowProcessState state = reflowProcessState.get();
  if (k != NULL)
  {
    if (state == ReflowProcessState::USER_INPUT)
    {
      leds.handleButtonStateChange(*k);
      oled.handleButtonStateChange(*k);
    }
    else if (state >= ReflowProcessState::PREHEAT && state <= ReflowProcessState::COOL)
    {
      if (k->first == ButtonKind::BACK && k->second.to == ButtonState::PRESSED)
      {
        // STOP REFLOW and restart
        reflowProcessState.set(ReflowProcessState::USER_INPUT);
        pidController.stop();
      }
    } else if (state == ReflowProcessState::DONE) {
      oled.handleButtonStateChange(*k);
    }
  }
  ReflowProcessState newState = reflowProcessState.get();

  if (newState != state)
  {
    Serial.println("State changed from " + String(STATE_STR(state)) + " to " + String(STATE_STR(newState)));
    // State changed from state to newState (user input or wifi input needs to be above here)
    if (newState == ReflowProcessState::PREHEAT) {

      tftDisplay.init(&chosenReflowProfile);
      chosenReflowProfile.start();
      // Start the PID
      pidController.start();
    }
  }
  state = newState;

  leds.loop();
  oled.loop();

  if (state >= ReflowProcessState::PREHEAT && state <= ReflowProcessState::COOL)
  {
    pidController.loop();
    ReflowStep step = chosenReflowProfile.reflowStep();

    if (step.state != newState)
    {
      reflowProcessState.set(step.state);
      if (step.state == ReflowProcessState::DONE)
      {
        pidController.stop();
      }
    }
  }
 

}
