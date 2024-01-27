#ifndef __oled_h
#define __oled_h


#include <Adafruit_SSD1306.h>


class OledDisplay
{
public:

    OledDisplay();
    void setup();
    void loop();
    void teardown();
    void drawDebug();
    void clearAll();
    void warning(String message);

private:
    Adafruit_SSD1306 display;

    void handleUserInputState();
    void handlePreheatState();
    void handleSoakState();
    void handleReflowState();
    void handleCoolingState();
    void handleFinishedState();
};

#endif