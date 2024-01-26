#ifndef __reflow_h__
#define __reflow_h__
#include "./common.h"
#include <EEPROM.h>
#include "StopWatch.h"


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
                                                 : state == USER_INPUT   ? "USER_INPUT"   \
                                                 : state == PREHEAT      ? "PREHEAT"      \
                                                 : state == SOAK         ? "SOAK"         \
                                                 : state == REFLOW       ? "REFLOW"       \
                                                 : state == COOL         ? "COOL"         \
                                                 : state == DONE         ? "DONE"         \
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
class ReflowProfile
{
public:
    ReflowProfile(ReflowStep steps[5], char name[20])
    {
        memcpy(this->steps, steps, 5 * sizeof(steps[0]));
        memcpy(this->name, name, PROFILE_SERIALIZED_NAME_SIZE);
    }
    ReflowStep steps[5];
    char name[20];
    StopWatch timer;

    void start()
    {
        timer = StopWatch(StopWatch::Resolution::MILLIS);
        timer.start();
    }

    /**
     * Calculates the current reflow step based on the elapsed time
    */
    ReflowStep curReflowStep() {
        int8_t reflowStep = curReflowStepIndexAt(timer.elapsed());
        if (reflowStep == -1) {
            return steps[4]; // DONE
        }
        return steps[reflowStep];
    }


    int8_t curReflowStepIndexAt(uint32_t elapsed) {
        for (int i = 0; i < 5; i++)
        {
            if (elapsed < steps[i].duration * 1000)
            {
                return i;
            }
            else
            {
                elapsed -= steps[i].duration * 1000;
            }
        }
        return -1; // we are done
    }

    float getTargetTemp()
    {
        uint32_t elapsedTime = timer.elapsed();
        uint8_t startTemp = 20; // always assume 20 degrees at the start
        int curStep = curReflowStepIndexAt(elapsedTime);
        if (curStep == -1) {
            return startTemp; // We are done return 20 degrees
        }
        uint32_t relativeElapsedTime = elapsedTime;
        for (int i=0; i<curStep; i++) {
            relativeElapsedTime -= steps[i].duration * 1000;
            startTemp = steps[i].targetTempAtEnd;
        }
        
        // Calculate percentage of current step
        float percentage = (float)relativeElapsedTime / (float)(steps[curStep].duration * 1000);
        return steps[curStep].calcTempAtPercentage(startTemp, percentage);
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
                (ReflowStepEaseFunction) b[PROFILE_SERIALIZED_NAME_SIZE + 2 + i * 3]);
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