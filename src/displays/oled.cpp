#include "./oled.h"
#include <Arduino.h>
#include "../globals.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

unsigned long lastProcessedReflowState = 0;
OledDisplay::OledDisplay()
{

    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void OledDisplay::setup()
{
    // Setup implementation
    bool initialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if (!initialized)
    {
        Serial.println("OLED Display failed to initialize");
    }
    else
    {
        Serial.println("OLED Display initialized");
    }
    display.setRotation(0);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Defaults to white

}

void OledDisplay::loop()
{

    StateChangeEvent<ReflowProcessState> *evt = reflowProcessState.getSince(lastProcessedReflowState);
    // REdraw only on reflow state change
    if (evt != NULL)
    {
        lastProcessedReflowState = millis();
        ReflowProcessState reflowState = reflowProcessState.get();
        if (reflowState == USER_INPUT)
        {
            handleUserInputState();
            return;
        }
        else if (reflowState == SOAK)
        {
            handleSoakState();
            return;
        }
        else if (reflowState == REFLOW)
        {
            handleReflowState();
            return;
        }
        else if (reflowState == ReflowProcessState::COOL)
        {
            handleCoolingState();
            return;
        }
        else if (reflowState == ReflowProcessState::DONE)
        {
            handleFinishedState();
            return;
        }
    }

    if (reflowProcessState.get() == PREHEAT)
    {
        drawDebug();
    }
    // Loop implementation
}

void OledDisplay::teardown()
{
    // delete display;
}

void OledDisplay::drawDebug()
{
    float sysVoltage = analogRef.calculateSystemVoltage();
    float inputVoltage = analogRef.calculateInputVoltage();
    int thermistor1Temp = thermistor1.getTemperature();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SysV: " + String(sysVoltage));
    display.setCursor(0, 20);
    display.println("In V: " + String(inputVoltage));
    display.setCursor(0, 40);
    display.println("C°: " + String(thermistor1Temp));
    display.display();
}

void OledDisplay::handleUserInputState()
{
    Serial.println("USER_INPUT_STATE");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);

    display.println("INPUT_STATE");
    display.display();
}

void OledDisplay::handlePreheatState()
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("PREHEATING");
    display.display();
}

void OledDisplay::handleSoakState()
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("SOAKING");
    display.display();
}

void OledDisplay::handleReflowState()
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("REFLOW");
    display.display();
}

void OledDisplay::handleCoolingState()
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("COOLING");
    display.display();
}

void OledDisplay::handleFinishedState()
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("Done :)");
    display.display();
}