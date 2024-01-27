#ifndef __oled_h
#define __oled_h
#include <Adafruit_SSD1306.h>
#include "../reflow.h"
#include "menustatemachine.h"

class OledDisplay {
    public:
        OledDisplay();
        void setup();
        void loop();
        void teardown();
        void handleButtonStateChange(Pair<ButtonKind, StateChangeEvent<ButtonState>> change);

    private:
        OledMenu *curMenu;
        Adafruit_SSD1306 display;
        void drawDebug();
        void handleUserInputState();
        void handleReflowState();

        void centerText(const char * text);
        void displayIndicators();
};



#endif