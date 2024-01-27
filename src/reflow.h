#ifndef __reflow_h__
#define __reflow_h__
#include "./common.h"
#include <EEPROM.h>
#include "StopWatch.h"
#include "thermistors/Thermistor.h"
#include "displays/oled.h"



extern Thermistor thermistor1;

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

            Serial.println(this->targetTempAtEnd);
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
    uint8_t preheatEndTime;
    uint8_t soakEndTime;
    uint8_t reflowEndTime;
    uint8_t coolEndTime;
    int totalDuration;

    float targetTempReflow;

    float percentage;

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
    StopWatch timer;

    void start()
    {
        timer = StopWatch(StopWatch::Resolution::SECONDS);
        timer.start();
        calculateEndTimes();
        checkTotalTime();
    }

    // Make sure the total time does not exceed 254 seconds

    //TODO: Fix overflow problem. Something is making the times and
    void checkTotalTime(){

    };

    void calculateEndTimes()
    {

        preheatEndTime = steps[0].duration;
        int soakEnd = steps[1].duration;
        soakEndTime = preheatEndTime + soakEnd;
        reflowEndTime = soakEndTime + steps[2].duration;
        coolEndTime = reflowEndTime + steps[3].duration;

        // reflowEndTime = soakEndTime + (steps[2].duration*1000);
        // coolEndTime = reflowEndTime + (steps[3].duration*1000);

        Serial.print("preheatEndTime: ");
        Serial.println(preheatEndTime);
        Serial.print("soakEndTime: ");
        Serial.println(soakEndTime);
        Serial.print("reflowEndTime: ");
        Serial.println(reflowEndTime);
        Serial.print("coolEndTime: ");
        Serial.println(coolEndTime);

        totalDuration = coolEndTime + steps[4].duration;

        Serial.print("totalDuration: ");
        Serial.println(totalDuration);
    }

    ReflowStep curReflowStep()
    {
        uint8_t elapsed = timer.elapsed();

        if (elapsed <= preheatEndTime)
        {
            return steps[0];
        }
        else if (elapsed < soakEndTime)
        {
            return steps[1];
        }
        else if (elapsed < reflowEndTime)
        {
            return steps[2];
        }
        else if (elapsed < coolEndTime)
        {
            return steps[3];
        }
        else
        {
            return steps[4];
            timer.stop();
            timer.reset();
        }
    }

    ReflowStep getPreviousSetep(ReflowStep step)
    {
        if (step.state == PREHEAT)
        {
            return steps[0];
        }
        else if (step.state == SOAK)
        {
            return steps[1];
        }
        else if (step.state == REFLOW)
        {
            return steps[2];
        }
        else if (step.state == COOL)
        {
            return steps[3];
        }
        else
        {

            return steps[4];
        }
    }

    float getTargetTemp()
    {
        uint32_t elapsedTime = timer.elapsed();
       // uint8_t startTemp = 20; // always assume 20 degrees at the start
        uint8_t startTemp=thermistor1.getTemperature();
        
        ReflowStep curStep = curReflowStep();
        ReflowStep prevStep = getPreviousSetep(curStep);

        int currenStepIndex = getCurrentStepIndex(curStep);
        int previousStepIndex = getCurrentStepIndex(prevStep);

        float relativeTIme = calculateCurrentStepRelativeTime(curStep);

        if (currenStepIndex == -1)
        {
            timer.reset();
            timer.stop();
            return startTemp; // We are done return 20 degrees
        }
        uint32_t relativeElapsedTime = elapsedTime;
        for (int i = 0; i < currenStepIndex; i++)
        {
            relativeElapsedTime -= steps[i].duration;
            startTemp = steps[i].targetTempAtEnd;
        }
        // float temp = curStep.calcTempAtPercentage(startTemp, percentage);

        // Serial.print("relativeElapsedTime: ");
        // Serial.println(relativeElapsedTime);
        // Serial.print("relativeTIme: ");
        // Serial.println(relativeTIme);
        // Serial.print("startTemp: ");
        // Serial.println(startTemp);

        percentage = (float)relativeElapsedTime / (float)(steps[currenStepIndex].duration);

        targetTempReflow = steps[currenStepIndex].calcTempAtPercentage(startTemp, percentage);

        return targetTempReflow;
    }

    int getCurrentStepIndex()
    {
        uint8_t elapsed = timer.elapsed();

        if (elapsed <= preheatEndTime)
        {
            return 0;
        }
        else if (elapsed < soakEndTime)
        {
            return 1;
        }
        else if (elapsed < reflowEndTime)
        {
            return 2;
        }
        else if (elapsed < coolEndTime)
        {
            return 3;
        }
        else
        {
            return 4;
        }
    }

    int getCurrentStepIndex(ReflowStep step)
    {
        if (step.state == PREHEAT)
        {
            return 0;
        }
        else if (step.state == SOAK)
        {
            return 1;
        }
        else if (step.state == REFLOW)
        {
            return 2;
        }
        else if (step.state == COOL)
        {
            return 3;
        }
        else
        {
            return 4;
        }
    }

    float getCurrentStepRelativeTime()
    {
        return calculateCurrentStepRelativeTime(curReflowStep());
    };

    float calculateCurrentStepRelativeTime(ReflowStep step)
    {

        uint8_t elapsed = timer.elapsed();

        switch (step.state)
        {
        case PREHEAT:
            return (float)elapsed;
        case SOAK:
            return (float)elapsed - preheatEndTime;
        case REFLOW:
            return (float)elapsed - soakEndTime;
        case COOL:
            return (float)elapsed - reflowEndTime;
        case DONE:
            return (float)elapsed - coolEndTime;
        }
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