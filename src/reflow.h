#ifndef __reflow_h__
#define __reflow_h__

// STATE MACHINE
enum ReflowProcessState {
    INITIALIZING, // default. This is the state the reflow oven/PCB is in when it is first turned on
    USER_INPUT, // The user is selecting a profile to use for the reflow process
    PREHEAT, // The oven/PCB is heating up to the preheat temperature
    SOAK, // The oven/PCB is heating up to the soak temperature
    REFLOW, // The oven/PCB is heating up to the reflow temperature
    COOL, // The oven/PCB is cooling down to the cool temperature
    DONE // The reflow process is complete
};

// Holds current reflow process state to be used by other classes 
// Such as the menu class to display the current state
extern ReflowProcessState reflowProcessState;

#endif