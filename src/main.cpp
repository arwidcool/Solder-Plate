#include <Arduino.h>
#include "ArduPID.h"
#include <Adafruit_ST7789.h> // Include the ST7789 library
#include <Adafruit_GFX.h>
#include <Tools/AnalogRef.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Tools/Thermistor.h>

enum Button
{
  UP,
  DOWN,
  BACK,
  SELECT,
  NONE
};

AnalogRef analogRef(5.0);

// LED pins
#define yellowLED 18
#define greenLED 19
#define redLED 20

// Button pins
#define upButton 21
#define downButton 22
#define backButton 23
#define selectButton 24

volatile bool upButtonPressed = false;
volatile bool downButtonPressed = false;
volatile bool backButtonPressed = false;
volatile bool selectButtonPressed = false;

volatile bool upButtonReleased = false;
volatile bool downButtonReleased = false;
volatile bool backButtonReleased = false;
volatile bool selectButtonReleased = false;

enum ButtonState
{
  IDLE,
  PRESSED,
  RELEASED
};

ButtonState upButtonState = IDLE;
ButtonState downButtonState = IDLE;
ButtonState backButtonState = IDLE;
ButtonState selectButtonState = IDLE;

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

// Initalize a 3950 100K thermistor with 2.5k reference resistor
Thermistor thermistor1(THERMISTOR1_PIN, 2500, {25, 100000, 86, 10000, 170, 1000});

ArduPID PID;

Button getPressedButton();
Button handleButtons();



void upButtonISR();
void downButtonISR();
void backButtonISR();
void selectButtonISR();

void i2cScanner();

void setup()
{

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed further, loop forever
  }

  analogWriteFrequency(64);


  display.setRotation(3);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(upButton), upButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(downButton), downButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(backButton), backButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(selectButton), selectButtonISR, FALLING);

  Serial.begin(9600);

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

  handleButtons();

  analogRef.calculate();

  float sysVoltage = analogRef.sysVoltage;
  float inputVoltage = analogRef.calculateInputVoltage();

  // Print the system voltage on the tft

  display.clearDisplay();
  Serial.print("Thermistor 1: ");
  Serial.println(thermistor1.getTemperature());
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
  display.println(thermistor1.getTemperature());

  display.display();

  // Serial.println("System voltage: ");

  // Serial.print("Output voltage: ");
  // Serial.println(sysVoltage);

  // Serial.print("Input voltage: ");
  // Serial.println(analogRef.calculateInputVoltage());
}

Button getPressedButton()
{
  if (upButtonPressed)
  {
    upButtonPressed = false;

    return UP;
  }
  else if (downButtonPressed)
  {
    downButtonPressed = false;

    return DOWN;
  }
  else if (backButtonPressed)
  {
    backButtonPressed = false;

    return BACK;
  }
  else if (selectButtonPressed)
  {
    selectButtonPressed = false;

    return SELECT;
  }

  return NONE;
}

Button handleButtons()
{

  Button pressedButton = getPressedButton();

  if (pressedButton != NONE)
  {
    Serial.print("Button pressed: ");
    switch (pressedButton)
    {
    case UP:
      Serial.println("UP");
      digitalWrite(yellowLED, HIGH);
      delay(100);
      digitalWrite(yellowLED, LOW);
      break;
    case DOWN:
      Serial.println("DOWN");
      digitalWrite(yellowLED, HIGH);
      delay(100);
      digitalWrite(yellowLED, LOW);
      break;
    case BACK:
      Serial.println("BACK");
      digitalWrite(redLED, HIGH);
      delay(100);
      digitalWrite(redLED, LOW);
      break;
    case SELECT:
      Serial.println("SELECT");
      analogWrite(greenLED, 20);
      delay(100);
      analogWrite(greenLED, 0);

      break;
    default:
      break;
    }
  }

  return pressedButton;
}

void upButtonISR()
{
  upButtonPressed = true;
}

void downButtonISR()
{
  downButtonPressed = true;
}

void backButtonISR()
{
  backButtonPressed = true;
}

void selectButtonISR()
{
  selectButtonPressed = true;
}

void i2cScanner()
{

  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000); // wait 5 seconds for next scan
}