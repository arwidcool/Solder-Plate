#include <Arduino.h>
#include "ArduPID.h"
#include <Adafruit_ST7789.h> // Include the ST7789 library
#include <Adafruit_GFX.h>
#include <voltageReference/AnalogRef.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <thermistors/Thermistor.h>
#include "buttons/Buttons.h"
#include "leds/leds.h"

// Define the analog ref used for the system voltage
AnalogRef analogRef(5.0);

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

// OLED display width and height, for a typical 128x64 display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initalize the 3950 100K thermistors with ACTUAL reference resistor measurnment(Measured between Left pin and GND when the board is powered off) using the default calibration data for 100K thermistor
Thermistor thermistor1(THERMISTOR1_PIN, 2500);
Thermistor thermistor2(THERMISTOR2_PIN, 2500);
Thermistor thermistor3(THERMISTOR3_PIN, 2500);
Thermistor thermistor4(THERMISTOR4_PIN, 2500);
Thermistor thermistor5(THERMISTOR5_PIN, 2500);
Thermistor thermistor6(THERMISTOR6_PIN, 9000);

// Initialize the buttons
Buttons buttons = Buttons();

// Define global for the Yellow LED because both up and down buttons use it
bool yellowLedON = false;

// Declare the PID
ArduPID PID;

void setup()
{

  Serial.begin(9600);

  Serial.println("Starting OLED");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(3);

  // Set PWM frequency to 64 kHz
  analogWriteFrequency(64);

  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  Serial.println("Starting LCD");

  // tft.init(240, 320); // The dimensions (width and height) of your display

  // // Set the rotation of the display if needed (0, 1, 2, or 3)
  // tft.setRotation(1);

  // // Fill the screen with a background color
  // tft.fillScreen(ST77XX_BLACK);

  // // Draw the lines of the gaph with 20 pixels from left side and 20 pixels from bottom just left side and bottom line using draw fast line
  // tft.drawLine(30, 30, 30, 210, ST77XX_WHITE);
  // tft.drawLine(30, 210, 300, 210, ST77XX_WHITE);

  // Set cursor to middle of screen and calculate center line from text vairable;
  // char *text = "Reflow quickchip 138c";
  // int16_t x1, y1;
  // uint16_t w, h;

  // tft.setTextSize(2);
  // tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  // tft.setCursor(160 - (w / 2), 0 + (h / 2));

  // // // Print text to screen
  //  tft.print(text);
  Serial.println("Ready!");
}

void loop()
{

  // Return the button that was pressed
  ButtonKind k = buttons.handleButtons();

  // Handle the buttons outside the main loop, only put functions in here for sanity and flow purposes
  buttons.handleButtonLEDs();

  float sysVoltage = analogRef.calculateSystemVoltage();
  float inputVoltage = analogRef.calculateInputVoltage();
  int thermistor1Temp = thermistor1.getTemperature();

  // Print the system voltage on the tft

  display.clearDisplay();
  Serial.print("Thermistor 1: ");
  Serial.println(thermistor1Temp);
  char *text = "Sys V: ";
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(text);
  display.setCursor(0, 40);
  display.println(sysVoltage);
  char *text2 = "In V: ";
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 60);
  display.println(text2);
  display.setCursor(0, 80);
  display.println(inputVoltage);
  char *text3 = "C:";
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 100);
  display.println(text3);
  display.setCursor(25, 100);
  display.println(thermistor1Temp);
  display.display();

  // Serial.println("System voltage: ");

  // Serial.print("Output voltage: ");
  // Serial.println(sysVoltage);

  // Serial.print("Input voltage: ");
  // Serial.println(analogRef.calculateInputVoltage());
}
