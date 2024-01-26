#include "Button.h"
#include <Arduino.h>

// Constructor
Button::Button(ButtonKind kind, uint8_t pin) : kind(kind), pin(pin), state(WrappedState<ButtonState>(ButtonState::IDLE))
{
    pinMode(pin, INPUT_PULLUP);
}

ButtonKind Button::getKind()
{
    return this->kind;
}

uint8_t Button::getPin()
{
    return this->pin;
}

StateChangeEvent<ButtonState> *Button::lastChange()
{
    return this->state.lastChangeEvent;
}

bool Button::loop()
{
    StateChangeEvent<ButtonState> *evt = NULL;

    if (digitalRead(this->pin) == LOW)
    {
        if (this->state.get() == ButtonState::IDLE && millis() - this->state.lastStateChangeTime > 50)
        {
            evt = this->state.set(ButtonState::PRESSED);
        }
    }
    else if (this->state.get() == ButtonState::PRESSED)
    {
        evt = this->state.set(ButtonState::RELEASED);
    }
    if (this->state.get() == ButtonState::RELEASED && millis() - this->state.lastStateChangeTime > 50)
    {
        evt = this->state.set(ButtonState::IDLE);
    }
    // Return true if the state changed
    return evt != NULL;
}