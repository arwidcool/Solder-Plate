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
#include "tools/ExecutionTimer.h"
#include "StopWatch.h"
#include "thermistors/Thermistor.h"

#define MOSTFET_PIN 17

double currentTemp = 0;
double targetTemp = 60;
double pwmValue = 255;

ExecutionTimer executionTimer;

Buttons buttons = Buttons();
LEDS leds = LEDS();
// Declare the PID
ArduPID PID;
OledDisplay oled = OledDisplay();

TFT_Display tftDisplay;
TemperatureController temperatureController;

StopWatch thermTimer = StopWatch();
StopWatch thermMilisTimer = StopWatch();

void setup()
{

  pinMode(MOSTFET_PIN, OUTPUT);
  analogWrite(MOSTFET_PIN, 255); // VERY IMPORTANT, DONT CHANGE! 255=off, 0=full power

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

  thermTimer.setResolution(StopWatch::Resolution::MILLIS);

  thermMilisTimer.setResolution(StopWatch::Resolution::MILLIS);
}
void loop()
{

  //  executionTimer.start();

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
        leds.reset();
      }
    }
    else if (state == ReflowProcessState::DONE)
    {
      oled.handleButtonStateChange(*k);
    }
  }
  ReflowProcessState newState = reflowProcessState.get();

  if (newState != state)
  {
    Serial.println("State changed from " + String(STATE_STR(state)) + " to " + String(STATE_STR(newState)));
    // State changed from state to newState (user input or wifi input needs to be above here)
    if (newState == ReflowProcessState::PREHEAT)
    {

      tftDisplay.init(&chosenReflowProfile);

      chosenReflowProfile.start();
      // Start the PID
      pidController.start();
      thermMilisTimer.start();
      thermTimer.start();
      Serial.println("Time,Therm1,Therm2,Therm3,Therm4,Therm5,Therm6,,Scaling1,Scaling2,Scaling3,Scaling4,Scaling5,Scaling6,Averaged1,Weighting1,Weighting2,Weighting3");
    }
  }
  state = newState;

  leds.loop();
  oled.loop();

  if (state >= ReflowProcessState::PREHEAT && state <= ReflowProcessState::COOL)
  {

    pidController.loop();
    // #ifdef DEBUG
    // pidController.debug();
    //   #endif
    tftDisplay.drawRealTemp(pidController.getInput(), chosenReflowProfile.getPercentage());
    ReflowStep step = chosenReflowProfile.reflowStep();
    // Here we draw the actual temp vs time to the display

    if (thermTimer.elapsed() > 1)
    {

      Serial.print(thermTimer.elapsed());
      Serial.print(",");
      float temp1 = thermistor1.getTemperature();
      float temp2 = thermistor2.getTemperature();
      float temp3 = thermistor3.getTemperature();
      float temp4 = thermistor4.getTemperature();
      float temp5 = thermistor5.getTemperature();
      float temp6 = thermistor6.getTemperature();
      Serial.print(temp1);
      Serial.print(",");
      Serial.print(temp2);
      Serial.print(",");
      Serial.print(temp3);
      Serial.print(",");
      Serial.print(temp4);
      Serial.print(",");
      Serial.print(temp5);
      Serial.print(",");
      Serial.print(temp6);
      Serial.print(",");
      Serial.print(thermistor1.scalingFactor);
      Serial.print(",");
      Serial.print(thermistor2.scalingFactor);
      Serial.print(",");
      Serial.print(thermistor3.scalingFactor);
      Serial.print(",");
      Serial.print(thermistor4.scalingFactor);
      Serial.print(",");
      Serial.print(thermistor5.scalingFactor);
      Serial.print(",");
      Serial.print(thermistor6.scalingFactor);
      Serial.print(",");

      float values[] = {temp2,temp3,temp4};
      float weights[] = {thermistor2.getWeightingFactor(),thermistor3.getWeightingFactor(),thermistor4.getWeightingFactor()};
      uint8_t length = 3;
      float averaged = TemperatureController::getWeightedAverage(values, weights, length);
      Serial.print(averaged);
      Serial.print(",");
      Serial.print(weights[0]);
      Serial.print(",");
      Serial.print(weights[1]);
      Serial.print(",");
      Serial.print(weights[2]);
      Serial.print(",");
      Serial.println();
      // Serial.print(",");
      // Serial.print(thermistor1.getWeightingFactor());
      // Serial.print(",");
      // Serial.print(thermistor2.getWeightingFactor());
      // Serial.print(",");
      // Serial.print(thermistor3.getWeightingFactor());
      // Serial.print(",");
      // Serial.print(thermistor4.getWeightingFactor());
    }

    if (step.state != newState)
    {
      reflowProcessState.set(step.state);
      if (step.state == ReflowProcessState::DONE)
      {
        pidController.stop();
      }
    }
  }

  //  executionTimer.stop();
}
