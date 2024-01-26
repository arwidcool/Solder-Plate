#include "Leds.h"
#include <Arduino.h>
#include "../globals.h"

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
void LEDS::handleButtonStateChange(Pair<ButtonKind, StateChangeEvent<ButtonState>> change)
{
    switch (change.first) {
        case ButtonKind::UP:
            if (change.second.to == ButtonState::PRESSED) {
                digitalWrite(yellowLED, HIGH);
            } else if (!yellowLEDSeleOn) {
                digitalWrite(yellowLED, LOW);
            }
            yellowLEDUPOn = change.second.to == ButtonState::PRESSED;
            break;
        case ButtonKind::DOWN:
            if (change.second.to == ButtonState::PRESSED) {
                digitalWrite(yellowLED, HIGH);
            } else if (!yellowLEDUPOn) {
                digitalWrite(yellowLED, LOW);
            }
            break;
        case ButtonKind::BACK:
            if (change.second.to == ButtonState::PRESSED) {
                digitalWrite(redLED, HIGH);
            } else {
                digitalWrite(redLED, LOW);
            }
            break;
        case ButtonKind::SELECT:
            if (change.second.to == ButtonState::PRESSED) {
                analogWrite(greenLED, 30);
            } else {
                digitalWrite(greenLED, LOW);
            }
            yellowLEDSeleOn = change.second.to == ButtonState::PRESSED;

            break;
        default:
            break;
    }
}