#ifndef __basestatechange_h__
#define __basestatechange_h__
#include <Arduino.h>

template <typename State>
class StateChangeEvent
{
public:
    StateChangeEvent(State from, State to) : from(from), to(to) {}
    StateChangeEvent(State from) : from(from), to(from) {}
    State from;
    State to;
};

template <typename State>
class WrappedState
{
public:
    WrappedState(State defaultState) : state(defaultState), lastChangeEvent(new StateChangeEvent<State>(defaultState)) {}
    StateChangeEvent<State> *set(State state)
    {
        if (this->state == state)
        {
            return NULL;
        }
        delete this->lastChangeEvent;
        lastChangeEvent = new StateChangeEvent<State>(this->state, state);
        this->state = state;
        this->lastStateChangeTime = millis();
        return lastChangeEvent;
    }
    State get() const
    {
        return this->state;
    }

    StateChangeEvent<State> *getSince(unsigned long time) const
    {
        if (this->lastStateChangeTime < time)
        {
            return NULL;
        }
        return lastChangeEvent;
    }
    State state;
    unsigned long lastStateChangeTime = 0;
    StateChangeEvent<State> *lastChangeEvent;
};

template <typename First, typename Second>
class Pair
{
public:
    Pair(){};
    Pair(First first, Second second) : first(first), second(second) {}
    First first;
    Second second;
};
enum ThermistorZ_Placement
{
    TOP,
    BOTTOM,
    ON_PCB

};

enum ThermistorXY_Placement
{
    MIDDLE,
    SIDE,
    MIDDLE_LOW_TEMP,
    MIDDLE_HIGH_TEMP,

};

#endif