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
#include "PID/PidController.h"
#include "reflow.h"


WrappedState<ReflowProcessState> reflowProcessState = WrappedState<ReflowProcessState>(INITIALIZING);

// Define the analog ref used for the system voltage
AnalogRef analogRef(5.0);

OledDisplay oled = OledDisplay();

// Calibration data for 100K thermistor
TempCalibration calibration_100K_3950 = {25, 100000, 86, 10000, 170, 1000};

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

PidController controller = PidController(&currentTemp, &pwmValue, &targetTemp);

// Initalize the 3950 100K thermistors with ACTUAL reference resistor measurnment(Measured between Left pin and GND when the board is powered off) using the default calibration data for 100K thermistor
Thermistor thermistor1(THERMISTOR1_PIN, 2500);
Thermistor thermistor2(THERMISTOR2_PIN, 2500);
Thermistor thermistor3(THERMISTOR3_PIN, 2500);
Thermistor thermistor4(THERMISTOR4_PIN, 2500);
Thermistor thermistor5(THERMISTOR5_PIN, 2500);
Thermistor thermistor6(THERMISTOR6_PIN, 9000);

// Initialize the buttons
Buttons buttons = Buttons();
LEDS leds = LEDS();
// Declare the PID



ReflowProfile profile = ReflowProfile(new ReflowStep[5]{
                                          ReflowStep(ReflowProcessState::PREHEAT, 2, 100),
                                          ReflowStep(ReflowProcessState::SOAK, 2, 140),
                                          ReflowStep(ReflowProcessState::REFLOW, 2, 160, EASE_IN_OUT),
                                          ReflowStep(ReflowProcessState::COOL, 2, 20),
                                          ReflowStep(ReflowProcessState::DONE, 0, 0)},
                                      "meow\0");

// Reflowprofile profile = Reflowprofile(new ReflowStep[5]{
//                                              ReflowStep(ReflowProcessState::PREHEAT, 20, 50),
//                                              ReflowStep(ReflowProcessState::SOAK, 35, 70),
//                                              ReflowStep(ReflowProcessState::REFLOW, 20, 85),
//                                              ReflowStep(ReflowProcessState::COOL, 20, 20),
//                                              ReflowStep(ReflowProcessState::DONE, 0, 0)},
//                                          "meow\0");

void setup()
{

  pinMode(MOSTFET_PIN, OUTPUT);
  analogWrite(MOSTFET_PIN, 255); // VERY IMPORTANT, DONT CHANGE!

  Serial.begin(9600);

  Serial.println("Starting OLED");
  // Set PWM frequency to 64 kHz
  analogWriteFrequency(64);
  buttons.setup();
  leds.setup();
  oled.setup();

  char name[20] = "meow\0";
  // ReflowProfile profile = ReflowProfile::fromEEPROM(0);

  // ReflowProfile profile = ReflowProfile(new ReflowStep[5] {
  //   ReflowStep(ReflowProcessState::PREHEAT, 30, 150),
  //   ReflowStep(ReflowProcessState::SOAK, 30, 180),
  //   ReflowStep(ReflowProcessState::REFLOW, 30, 220),
  //   ReflowStep(ReflowProcessState::COOL, 30, 100),
  //   ReflowStep(ReflowProcessState::DONE, 0, 0)
  // }, name);
  // profile.toEEPROM(0);

  Serial.println(profile.name);
  for (int i = 0; i < 5; i++)
  {
    Serial.print(profile.steps[i].duration);
    Serial.print(" ");
    Serial.println(profile.steps[i].targetTempAtEnd);
  }
  reflowProcessState = USER_INPUT;
  profile.start();
}

void loop()
{

  // Return the button that changed state
  Pair<ButtonKind, StateChangeEvent<ButtonState>> *k = buttons.handleButtons();

  if (k != NULL)
  {
    leds.handleButtonStateChange(*k);

    if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::SELECT, ButtonState::PRESSED))
    {
      reflowProcessState.set(ReflowProcessState::PREHEAT);
    }
    else if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::BACK, ButtonState::PRESSED))
    {
      reflowProcessState.set(ReflowProcessState::USER_INPUT);
    }
    else if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::UP, ButtonState::PRESSED))
    {
      reflowProcessState.set(ReflowProcessState::COOL);
    }
    else if (ISBUTTONMIGRATEDTOSTATE(*k, ButtonKind::DOWN, ButtonState::PRESSED))
    {
      reflowProcessState.set(ReflowProcessState::REFLOW);
    }
  }

  ReflowStep step = profile.curReflowStep();

  if (step.state != reflowProcessState.get())
  {
    reflowProcessState.set(step.state);
  }

  if (step.state == ReflowProcessState::DONE)
  {
    if (controller.started)
    {
      controller.stop();
    }
    Serial.println("DONE");
    analogWrite(MOSTFET_PIN, 255);
  }

  else
  {

    // targetTemp = profile.getTargetTemp();
    targetTemp = profile.getTargetTemp();
    currentTemp = thermistor1.getTemperature();
    controller.compute();

    analogWrite(MOSTFET_PIN, pwmValue);

    // controller.debug();

    char myCharArray[50]; // Ensure this array is large enough to hold the string plus the null terminator

    String(STATE_STR(step.state)).toCharArray(myCharArray, sizeof(myCharArray));

    controller.debug2(myCharArray);

    ;
    // Serial.print(String(STATE_STR(step.state)) + " " + String(step.duration) + " " + String(step.targetTempAtEnd) + " " + String(profile.timer.elapsed()) + " Step Time " + String(profile.getCurrentStepRelativeTime()) + " Target Temp:" + String(profile.targetTempReflow) + " % " + String(profile.percentage) + "\r");
  };

  // Serial.print("Targt temp: ");
  // Serial.print(targetTemp);

  oled.drawDebug();

  //   if (step.state == ReflowProcessState::DONE) {
  //     profile.start();
  //     return;
  //   }
  // Serial.print(String(STATE_STR(step.state)) + " " + String(step.duration) + " " + String(step.targetTempAtEnd) + " " + " " + String(profile.timer.elapsed()) + "Step Time" + String(profile.getCurrentStepRelativeTime()) + " Current Temp:" + String(profile.getCurrentTemp()) +"\r");
  // String(profile.getTargetTemp()) +

  // Serial.print(String(STATE_STR(step.state)) + " " + String(step.duration) + " " + String(step.targetTempAtEnd) + " " + String(profile.getTargetTemp())+"\r");
}
