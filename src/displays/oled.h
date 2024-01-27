#ifndef __oled_h
#define __oled_h
#include <Adafruit_SSD1306.h>
#include "../reflow.h"
#include "menustatemachine.h"
enum DisplayTextAlignment {
    START,
    CENTER,
    END
};
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
        void centerText(const char * text) {
            centerText(text, DisplayTextAlignment::CENTER, DisplayTextAlignment::CENTER);
        }
        void centerText(const char * text, DisplayTextAlignment horizontal, DisplayTextAlignment vertical);
        void displayIndicators();
        void handleDrawThermistorMenu(OledMenuItem item);
};

#endif