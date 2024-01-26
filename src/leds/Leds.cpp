#include "Leds.h"
#include <Arduino.h>

bool yellowLEDUPOn = false;
bool yellowLEDSeleOn = false;
LEDS::LEDS()
{
    // Constructor implementation
}

void LEDS::setup()
{
    pinMode(yellowLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
}
void LEDS::handleButtonStateChange(ButtonStateChange change)
{
    switch (change.kind) {
        case ButtonKind::UP:
            if (change.to == ButtonState::PRESSED) {
                digitalWrite(yellowLED, HIGH);
            } else if (!yellowLEDSeleOn) {
                digitalWrite(yellowLED, LOW);
            }
            yellowLEDUPOn = change.to == ButtonState::PRESSED;
            break;
        case ButtonKind::DOWN:
            if (change.to == ButtonState::PRESSED) {
                digitalWrite(greenLED, HIGH);
            } else {
                digitalWrite(greenLED, LOW);
            }
            break;
        case ButtonKind::BACK:
            if (change.to == ButtonState::PRESSED) {
                digitalWrite(redLED, HIGH);
            } else {
                digitalWrite(redLED, LOW);
            }
            break;
        case ButtonKind::SELECT:
            if (change.to == ButtonState::PRESSED) {
                digitalWrite(yellowLED, HIGH);
            } else if (!yellowLEDUPOn) {
                digitalWrite(yellowLED, LOW);
            }
            yellowLEDSeleOn = change.to == ButtonState::PRESSED;

            break;
        default:
            break;
    }
}