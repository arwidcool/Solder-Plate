#ifndef __reflow_h__
#define __reflow_h__
#include "./common.h"
#include <EEPROM.h>
#include "StopWatch.h"
#include "thermistors/Thermistor.h"
#include "displays/oled.h"

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
    EASE_OUT
};
class ReflowStep
{
public:
    ReflowStep() {}
    ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTempAtEnd) : state(state), duration(time), targetTempAtEnd(targetTempAtEnd), easeFunction(LINEAR) {}
    ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTempAtEnd, ReflowStepEaseFunction fn) : state(state),
                                                                                                             duration(time),
                                                                                                             targetTempAtEnd(targetTempAtEnd),
                                                                                                             easeFunction(fn)
    {
    }
    uint8_t duration;
    uint8_t targetTempAtEnd;
    ReflowProcessState state;
    ReflowStepEaseFunction easeFunction;

    float calcTempAtPercentage(uint8_t startTemp, float percentage)
    {
        switch (this->easeFunction)
        {
        case LINEAR:
            return startTemp + (this->targetTempAtEnd - startTemp) * percentage;

        case EASE_IN_OUT:
            return startTemp + (this->targetTempAtEnd - startTemp) * -(cos(percentage * PI) - 1) / (double)2;
        case EASE_IN:
            return startTemp + (this->targetTempAtEnd - startTemp) * (1 - cos(percentage * PI / (double)2));

        case EASE_OUT:
            return startTemp + (this->targetTempAtEnd - startTemp) * (sin(percentage * PI / (double)2));
        }
    }
};

#define PROFILE_SERIALIZED_SIZE 40
#define PROFILE_SERIALIZED_NAME_SIZE 20
#define STEPINDEX(step) (step.state - PREHEAT)

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
    }
    ReflowStep steps[5];
    char name[20];
    uint32_t endTimes[5] = {0};
    uint32_t startTimes[5] = {0};
    StopWatch timer;

    void start()
    {
        timer = StopWatch(StopWatch::Resolution::MILLIS);
        timer.start();
        calculateTimes();
    }

    void calculateTimes()
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
            if (elapsedMS >= startTimes[i] * 1000 && elapsedMS < endTimes[i] * 1000)
            {
                // Serial.println(String(elapsedMS) + " " + String(startTimes[i] * 1000) + " " + String(endTimes[i] * 1000) + " " + String(i) + " " + String(steps[i].state));
                return steps[i];
            }
        }
        // Serial.println("AAA");
        return steps[4]; // DONE by default
    }

    float getPercentage() {
        return (float)timer.elapsed() / (float)(endTimes[4] * 1000);
    }

    float getTargetTemp()
    {
        if (!timer.isRunning())
        {
            return 20;
        }
        return getTargetTemp(timer.elapsed());        
    }
    float getTargetTemp(uint32_t elapsedMS) {
        uint8_t startTemp = 20; // always assume 20 degrees at the start

        ReflowStep curStep = reflowStep(elapsedMS);
        if (curStep.state > PREHEAT)
        {
            startTemp = steps[STEPINDEX(curStep) - 1].targetTempAtEnd;
        }

        // startTemp => 20 or the targetTempAtEnd of the previous step

        uint32_t startTimeMS = startTimes[STEPINDEX(curStep)] * 1000;

        uint32_t relativeElapsedTime = elapsedMS - startTimeMS;

        float percentage = (float)relativeElapsedTime / ((float)(curStep.duration) * 1000);
        // Serial.println(String(percentage)+ "%" + String(STATE_STR(curStep.state)) + " Elapsed: " + String(elapsedMS) + " ___ " + String(curStep.duration  * 1000));
        return curStep.calcTempAtPercentage(startTemp, percentage);
    }

    

    /**
     * @brief Get the Target Temp At Process Percentage.
     * @param processPercentage a number between 0 and 1. 0 is the start of the process, 1 is the end of the process
     * @return float the target temperature at the given percentage of the full process
    */
    float getTargetTempFromPercentage(double processPercentage)
    {
        uint16_t duration = endTimes[4];
        uint8_t startTemp = 20; // always assume 20 degrees at the start
        return getTargetTemp(duration * 1000 * processPercentage);
    }

    uint8_t getCurrentStepRelativeTime()
    {
        uint32_t elapsedMS = timer.elapsed();
        uint16_t startTimeMS = startTimes[STEPINDEX(reflowStep())] * 1000;
        return (elapsedMS - startTimeMS) / 1000;
    }











    void toBuffer(uint8_t *b)
    {
        memset(b, 0, PROFILE_SERIALIZED_SIZE);
        memcpy(b, name, PROFILE_SERIALIZED_NAME_SIZE);
        for (int i = 0; i < 5; i++)
        {
            b[PROFILE_SERIALIZED_NAME_SIZE + i * 3] = steps[i].duration;
            b[PROFILE_SERIALIZED_NAME_SIZE + 1 + i * 3] = steps[i].targetTempAtEnd;
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