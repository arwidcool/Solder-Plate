#include "Button.h"
#include <Arduino.h>


// Constructor
Button::Button(ButtonKind kind, uint8_t pin) : kind(kind), pin(pin), state(ButtonState::IDLE) {
    pinMode(pin, INPUT_PULLUP);
    lastStateChangeTime = 0;
}

ButtonKind Button::getKind() {
    return this->kind;
}

uint8_t Button::getPin() {
    return this->pin;
}

ButtonState Button::getState() {
    return this->state;
}

ButtonState Button::setState(ButtonState state) {
    lastStateChangeTime = millis();
    this->state = state;
    return this->state;
}

void Button::loop() {
    if (digitalRead(this->pin) == LOW) {
        if (this->state == ButtonState::IDLE && millis() - lastStateChangeTime > 100) {
            this->setState(ButtonState::PRESSED);
        }
    } else if (this->state == ButtonState::PRESSED) {
        this->setState(ButtonState::RELEASED);
    }
    if (this->state == ButtonState::RELEASED && millis() - lastStateChangeTime > 100) {
        this->setState(ButtonState::IDLE);
    }
}