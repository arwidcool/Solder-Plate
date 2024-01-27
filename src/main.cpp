#include <Arduino.h>
#include "ArduPID.h"
#include <Adafruit_ST7789.h> // Include the ST7789 library
#include <Adafruit_GFX.h>
#include <voltageReference/AnalogRef.h>
#include <Wire.h>
#include <thermistors/Thermistor.h>
#include "buttons/Buttons.h"
#include "leds/leds.h"
#include "reflow.h"
#include "displays/oled.h"
#include "PID/PidController.h"
#include "globals.h"
#include "EEPROMDataManager.h"

// LCD display pins
#define TFT_CS 7
#define TFT_RST 12
#define TFT_DC 14
#define MOSI 4 // MOSI
#define SCK 6  // SCK
// Create an instance of the Adafruit ST7789 class using the custom SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, MOSI, SCK, TFT_RST);

#define MOSTFET_PIN 17

double currentTemp = 0;
double targetTemp = 60;
double pwmValue = 255;

Buttons buttons = Buttons();
LEDS leds = LEDS();
// Declare the PID
ArduPID PID;
OledDisplay oled = OledDisplay();



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

  reflowProcessState.set(USER_INPUT);
}
void loop()
{

  // Return the button that changed state
  Pair<ButtonKind, StateChangeEvent<ButtonState>> *k = buttons.handleButtons();
  ReflowProcessState state = reflowProcessState.get();
  if (k != NULL)
  {
    leds.handleButtonStateChange(*k);
    if (state == USER_INPUT)
    {
      oled.handleButtonStateChange(*k);
    }
    else if (state >= PREHEAT && state <= COOL)
    {
      if (k->first == ButtonKind::BACK && k->second.to == ButtonState::PRESSED)
      {
        // STOP REFLOW and restart
        reflowProcessState.set(USER_INPUT);
        pidController.stop();
      }
    }
  }
  ReflowProcessState newState = reflowProcessState.get();
  if (newState != state) {
    Serial.println("State changed from " + String(STATE_STR(state)) + " to " + String(STATE_STR(newState)));
    // State changed from state to newState (user input or wifi input needs to be above here)
    if (newState == PREHEAT) {
      chosenReflowProfile.start();
      pidController.start();
    }
  }
  state = newState;

  oled.loop();

  if (state >= PREHEAT && state <= COOL)
  {
    pidController.loop();
    ReflowStep step = chosenReflowProfile.curReflowStep();

    if (step.state != newState)
    {
      reflowProcessState.set(step.state);
    }
  }

  if (state == DONE)
  {
    // TODO: BUZZER
    pidController.stop();
    reflowProcessState.set(USER_INPUT);
  }
  

  // if (step.state == ReflowProcessState::DONE) {
  //   profile.start();
  //   return;
  // }
  // Serial.print(String(STATE_STR(step.state)) + " " + String(step.duration) + " " + String(step.targetTempAtEnd) + " " + String(profile.getTargetTemp())+"\r");
}
