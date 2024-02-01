#ifndef __reflow_h__
#define __reflow_h__
#include "./common.h"
#include <EEPROM.h>
#include "StopWatch.h"
#include "thermistors/Thermistor.h"
//#include "thermistors/TemperatureController.h"

// STATE MACHINE
enum ReflowProcessState
{
    INITIALIZING, // default. This is the state the reflow oven/PCB is in when it is first turned on
    USER_INPUT,   // The user is selecting a profile to use for the reflow process
    PREHEAT,      // The oven/PCB is heating up to the preheat temperature
    SOAK,         // The oven/PCB is heating up to the soak temperature
    REFLOW,       // The oven/PCB is heating up to the reflow temperature
    COOL,         // The oven/PCB is cooling down to the cool temperature
    DONE          // The reflow process is complete
};

#define STATE_STR(state) (state == INITIALIZING ? "INITIALIZING" \
                          : state == USER_INPUT ? "USER_INPUT"   \
                          : state == PREHEAT    ? "PREHEAT"      \
                          : state == SOAK       ? "SOAK"         \
                          : state == REFLOW     ? "REFLOW"       \
                          : state == COOL       ? "COOL"         \
                          : state == DONE       ? "DONE"         \
                                                : "UNKNOWN")

enum ReflowStepEaseFunction
{
    LINEAR,
    EASE_IN_OUT,
    EASE_IN,
    EASE_OUT,
    HALF_SINE,
    SLOW_RAMP_HOLD,
    MID_RAMP_HOLD,
    FAST_RAMP_HOLD
};

class ReflowStep
{
public:
    ReflowStep() ;
    ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTemp) ;
    ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTemp, ReflowStepEaseFunction fn);
  
    uint8_t duration;
    uint8_t targetTemp;
    ReflowProcessState state;
    ReflowStepEaseFunction easeFunction;

    float calcTempAtPercentage(uint8_t startTemp, float percentage);
};



#define PROFILE_SERIALIZED_SIZE 40
#define PROFILE_SERIALIZED_NAME_SIZE 20
#define STEPINDEX(step) (step.state - PREHEAT)
#define TOMILLIS(x) (((uint32_t)x) * (uint32_t)1000)


class ReflowProfile
{

public:
    ReflowProfile(ReflowStep steps[5], char name[20]);
    ReflowStep steps[5];
    char name[20];
    uint16_t endTimes[5] = {0};
    uint16_t startTimes[5] = {0};
    uint8_t endTemps[5] = {0};
    uint8_t startTemps[5] = {0};
    StopWatch timer;

    void start();

    void calculateValues();

    ReflowStep reflowStep();

    ReflowStep reflowStep(uint32_t elapsedMS);
    float getPercentage();
    float getTargetTemp();
    float getTargetTemp(uint32_t elapsedMS);

    /**
     * @brief Get the Target Temp At Process Percentage.
     * @param processPercentage a number between 0 and 1. 0 is the start of the process, 1 is the end of the process
     * @return float the target temperature at the given percentage of the full process
     */
    float getTargetTempFromPercentage(double processPercentage);
    uint8_t getCurrentStepRelativeTime();

    void toBuffer(uint8_t *b);

    void toEEPROM(uint8_t index);
    static ReflowProfile fromBuffer(const uint8_t *b);

    static ReflowProfile fromEEPROM(uint8_t index);
};

#endif