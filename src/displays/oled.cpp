#include "./oled.h"
#include <Arduino.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
OledDisplay::OledDisplay() {

    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void OledDisplay::setup() {
    // Setup implementation
    bool initialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if (!initialized) {
        Serial.println("OLED Display failed to initialize");
    } else {
        Serial.println("OLED Display initialized");
    }
    display.setRotation(0);
}

void OledDisplay::loop() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    if (reflowProcessState == INITIALIZING) {
        display.println("INITIALIZING");
    } else  if (reflowProcessState == USER_INPUT) {
        display.println("USER_INPUT");
    } else if (reflowProcessState == PREHEAT) {
        display.println("PREHEAT");
    } else if (reflowProcessState == SOAK) {
        display.println("SOAK");
    } else if (reflowProcessState == REFLOW) {
        display.println("REFLOW");
    } else if (reflowProcessState == COOL) {
        display.println("COOL");
    } else if (reflowProcessState == DONE) {
        display.println("DONE");
    }
    display.println("zz");
    display.display();
    // Loop implementation
}

void OledDisplay::teardown() {
   // delete display;
}
