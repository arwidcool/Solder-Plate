#ifndef _REFLOW_PROCESSSTATE_H_
#define _REFLOW_PROCESSSTATE_H_
#include "./common.h"
#include <EEPROM.h>
#include "StopWatch.h"
#include "PID/PidController.h"

extern PidController pidController;

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
    ReflowStep() {}
    ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTemp) : state(state), duration(time), targetTemp(targetTemp), easeFunction(LINEAR) {}
    ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTemp, ReflowStepEaseFunction fn) : state(state),
                                                                                                        duration(time),
                                                                                                        targetTemp(targetTemp),
                                                                                                        easeFunction(fn)
    {
    }
    uint8_t duration;
    uint8_t targetTemp;
    ReflowProcessState state;
    ReflowStepEaseFunction easeFunction;

    float calcTempAtPercentage(uint8_t startTemp, float percentage)
    {
        switch (this->easeFunction)
        {
        case LINEAR:
            return startTemp + (this->targetTemp - startTemp) * percentage;
        case EASE_IN_OUT:
            return startTemp + (this->targetTemp - startTemp) * -(cos(percentage * PI) - 1) / (double)2;
        case EASE_IN:
            return startTemp + (this->targetTemp - startTemp) * (1 - cos(percentage * PI / (double)2));
        case EASE_OUT:
            return startTemp + (this->targetTemp - startTemp) * (sin(percentage * PI / (double)2));
        case HALF_SINE:
            return startTemp + (this->targetTemp - startTemp) * (sin(percentage * PI));

        case MID_RAMP_HOLD:

            if (percentage <= 0.50)
            {
                // Ramp up to the target temperature over the first half of the time
                return startTemp + (this->targetTemp - startTemp) * (percentage / 0.50);
            }
            else
            {
                // Hold at the target temperature for the remaining half of the time
                return this->targetTemp;
            }
        }
    }
};
#endif

#ifndef __reflow_h__
#define __reflow_h__

#define PROFILE_SERIALIZED_SIZE 40
#define PROFILE_SERIALIZED_NAME_SIZE 20
#define STEPINDEX(step) (step.state - PREHEAT)
#define TOMILLIS(x) (((uint32_t)x) * (uint32_t)1000)

class ReflowProfile
{
public:
    ReflowProfile(ReflowStep steps[5], char name[20])
    {

        for (int i = 0; i < 5; i++)
        {
            this->steps[i] = steps[i];
        }

        for (int i = 0; i < 20; i++)
        {
            this->name[i] = name[i];
        }
        calculateValues();
    }
    ReflowStep steps[5];
    char name[20];
    uint16_t endTimes[5] = {0};
    uint16_t startTimes[5] = {0};
    uint8_t endTemps[5] = {0};
    uint8_t startTemps[5] = {0};
    StopWatch timer;
    uint8_t plateTemp = 0;

    void start(uint8_t plateTemp)
    {
        timer = StopWatch(StopWatch::MILLIS);
        timer.start();
        this->plateTemp = plateTemp;
    }

    void calculateValues()
    {
        endTimes[0] = steps[0].duration;
        endTimes[1] = endTimes[0] + steps[1].duration;
        endTimes[2] = endTimes[1] + steps[2].duration;
        endTimes[3] = endTimes[2] + steps[3].duration;
        endTimes[4] = endTimes[3] + steps[4].duration;

        startTimes[0] = 0;
        startTimes[1] = endTimes[0];
        startTimes[2] = endTimes[1];
        startTimes[3] = endTimes[2];
        startTimes[4] = endTimes[3];

        // We will grab the current PCB temp from the PID as the start temp otherwise the PID will be off

            startTemps[0] = 20;
    

        endTemps[0] = steps[0].calcTempAtPercentage(startTemps[0], 1);
        endTemps[1] = steps[1].calcTempAtPercentage(endTemps[0], 1);
        endTemps[2] = steps[2].calcTempAtPercentage(endTemps[1], 1);
        endTemps[3] = steps[3].calcTempAtPercentage(endTemps[2], 1);
        endTemps[4] = steps[4].calcTempAtPercentage(endTemps[3], 1);

        startTemps[1] = endTemps[0];
        startTemps[2] = endTemps[1];
        startTemps[3] = endTemps[2];
        startTemps[4] = endTemps[3];
    }

    ReflowStep reflowStep()
    {
        if (!timer.isRunning())
        {
            return steps[0];
        }
        return reflowStep(timer.elapsed());
    }

    ReflowStep reflowStep(uint32_t elapsedMS)
    {
        for (int i = 0; i < 5; i++)
        {
            if (elapsedMS >= TOMILLIS(startTimes[i]) && elapsedMS < TOMILLIS(endTimes[i]))
            {
                return steps[i];
            }
        }
        return steps[4]; // DONE by default
    }

    float getPercentage()
    {
        return timer.elapsed() / (double)TOMILLIS(endTimes[4]);
    }

    float getTargetTemp()
    {
        if (!timer.isRunning())
        {
            return startTemps[0];
        }
        return getTargetTemp(timer.elapsed());
    }
    float getTargetTemp(uint32_t elapsedMS)
    {
        ReflowStep curStep = reflowStep(elapsedMS);

        uint32_t startTimeMS = TOMILLIS(startTimes[STEPINDEX(curStep)]);
        uint32_t relativeElapsedTime = elapsedMS - startTimeMS;

        float stepPercentage = relativeElapsedTime / (double)TOMILLIS(curStep.duration);
        return curStep.calcTempAtPercentage(startTemps[STEPINDEX(curStep)], stepPercentage);
    }

    /**
     * @brief Get the Target Temp At Process Percentage.
     * @param processPercentage a number between 0 and 1. 0 is the start of the process, 1 is the end of the process
     * @return float the target temperature at the given percentage of the full process
     */
    float getTargetTempFromPercentage(double processPercentage)
    {
        return getTargetTemp(TOMILLIS(endTimes[4]) * processPercentage);
    }

    uint8_t getCurrentStepRelativeTime()
    {
        uint32_t elapsedMS = timer.elapsed();
        uint32_t startTimeMS = TOMILLIS(startTimes[STEPINDEX(reflowStep())]);
        return (elapsedMS - startTimeMS) / 1000;
    }

    void toBuffer(uint8_t *b)
    {
        memset(b, 0, PROFILE_SERIALIZED_SIZE);
        memcpy(b, name, PROFILE_SERIALIZED_NAME_SIZE);
        for (int i = 0; i < 5; i++)
        {
            b[PROFILE_SERIALIZED_NAME_SIZE + i * 3] = steps[i].duration;
            b[PROFILE_SERIALIZED_NAME_SIZE + 1 + i * 3] = steps[i].targetTemp;
            b[PROFILE_SERIALIZED_NAME_SIZE + 2 + i * 3] = steps[i].easeFunction;
        }
    }

    void toEEPROM(uint8_t index)
    {
        uint8_t b[40];
        toBuffer(b);
        EEPROM.put(index * PROFILE_SERIALIZED_SIZE, b);
    }

    static ReflowProfile fromBuffer(const uint8_t *b)
    {
        char name[PROFILE_SERIALIZED_NAME_SIZE];
        memcpy(name, b, PROFILE_SERIALIZED_NAME_SIZE);
        ReflowStep steps[5] = {};
        for (int i = 0; i < 5; i++)
        {
            steps[i] = ReflowStep(
                (ReflowProcessState)(i + PREHEAT),
                b[PROFILE_SERIALIZED_NAME_SIZE + i * 3],
                b[PROFILE_SERIALIZED_NAME_SIZE + 1 + i * 3],
                (ReflowStepEaseFunction)b[PROFILE_SERIALIZED_NAME_SIZE + 2 + i * 3]);
        }
        return ReflowProfile(steps, name);
    }

    static ReflowProfile fromEEPROM(uint8_t index)
    {
        uint8_t b[40];
        EEPROM.get(index * PROFILE_SERIALIZED_SIZE, b);
        return fromBuffer(b);
    }
};

#endif