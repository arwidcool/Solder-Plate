#ifndef __basestatechange_h__
#define __basestatechange_h__
#include <Arduino.h>
template <typename State>
class StateChangeEvent
{
public:
    StateChangeEvent(State from, State to) : from(from), to(to) {}
    State from;
    State to;
};

template <typename State>
class WrappedState
{
public:
    WrappedState(State defaultState) : state(defaultState), lastChangeEvent(new StateChangeEvent<State>(defaultState, defaultState)) {}
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
    State get()
    {
        return this->state;
    }
    State state;
    unsigned long lastStateChangeTime = 0;
    StateChangeEvent<State> *lastChangeEvent;
};

template <typename First, typename Second>
class Pair
{
public:
    Pair(First first, Second second) : first(first), second(second) {}
    First first;
    Second second;
};
#endif