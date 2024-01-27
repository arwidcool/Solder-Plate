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


Buttons buttons = Buttons();
LEDS leds = LEDS();
// Declare the PID
ArduPID PID;
OledDisplay oled = OledDisplay();


void setup()
{

  Serial.begin(38400);

  Serial.println("Starting OLED");
  // Set PWM frequency to 64 kHz
  analogWriteFrequency(64);
  buttons.setup();
  leds.setup();
  oled.setup();
  eepromDataManager.setup();
  
  reflowProcessState = USER_INPUT;
  
}

void loop()
{

  // Return the button that changed state
  Pair<ButtonKind, StateChangeEvent<ButtonState>> *k = buttons.handleButtons();

  if (k != NULL) {
    leds.handleButtonStateChange(*k);
    oled.handleButtonStateChange(*k);

    // if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::SELECT, ButtonState::PRESSED)) {
    //   reflowProcessState.set(ReflowProcessState::PREHEAT);
    // } else if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::BACK, ButtonState::PRESSED)) {
    //   reflowProcessState.set(ReflowProcessState::USER_INPUT);
    // } else if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::UP, ButtonState::PRESSED)) {
    //   reflowProcessState.set(ReflowProcessState::COOL);
    // } else if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::DOWN, ButtonState::PRESSED)) {
    //   reflowProcessState.set(ReflowProcessState::REFLOW);
    // }
  }

  // ReflowStep step = profile.curReflowStep();
  // if (step.state != reflowProcessState.get()) {
  //   reflowProcessState.set(step.state);
  // }

   oled.loop();
  // if (step.state == ReflowProcessState::DONE) {
  //   profile.start();
  //   return;
  // }
  // Serial.print(String(STATE_STR(step.state)) + " " + String(step.duration) + " " + String(step.targetTempAtEnd) + " " + String(profile.getTargetTemp())+"\r");
  
}

