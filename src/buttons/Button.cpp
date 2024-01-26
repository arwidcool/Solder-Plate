#include "Button.h"
#include <Arduino.h>


// Constructor
Button::Button(ButtonKind kind, uint8_t pin) : kind(kind), pin(pin), state(ButtonState::IDLE) {
    pinMode(pin, INPUT_PULLUP);
    this->change = new ButtonStateChange(this->kind, ButtonState::IDLE, ButtonState::IDLE);
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

ButtonStateChange* Button::lastChange() {
    return this->change;
}

void Button::setState(ButtonState state) {
    if (this->state != state) {
        delete this->change; // clear memory
        this->change = new ButtonStateChange(this->kind, this->state, state);
        lastStateChangeTime = millis();
        this->state = state;
    }
}

bool Button::loop() {
    ButtonState prev = this->state;
    if (digitalRead(this->pin) == LOW) {
        if (this->state == ButtonState::IDLE && millis() - lastStateChangeTime > 50) {
            this->setState(ButtonState::PRESSED);
        }
    } else if (this->state == ButtonState::PRESSED) {
        this->setState(ButtonState::RELEASED);
    }
    if (this->state == ButtonState::RELEASED && millis() - lastStateChangeTime > 50) {
        this->setState(ButtonState::IDLE);
    }

    // Return true if the state changed
    return prev != this->state;
}