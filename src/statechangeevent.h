#ifndef __basestatechange_h__
#define __basestatechange_h__
#include <Arduino.h>
template <typename Kind, typename State>
class StateChangeEvent {
    public:
        StateChangeEvent(Kind kind, State from, State to) : kind(kind), from(from), to(to) {}
        Kind kind;
        State from;
        State to;

};


template <typename Kind, typename State>
class WrappedState {
    public:
        WrappedState(Kind kind, State defaultState) : kind(kind), state(defaultState), lastChangeEvent(new StateChangeEvent<Kind, State>(kind, defaultState, defaultState)) {}
        Kind kind;
        StateChangeEvent<Kind, State> * set(State state) {
            if (this->state == state) {
                return NULL;
            }
            delete this->lastChangeEvent;
            lastChangeEvent = new StateChangeEvent<Kind, State>(kind, this->state, state);
            this->state = state;
            this->lastStateChangeTime = millis();
            return lastChangeEvent;
        }
        State get() {
            return this->state;
        }
        State state;
        unsigned long lastStateChangeTime = 0;
        StateChangeEvent<Kind, State> * lastChangeEvent;
};
#endif