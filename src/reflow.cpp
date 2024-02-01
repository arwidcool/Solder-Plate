#include "reflow.h"

//extern TemperatureController tempController;

ReflowStep::ReflowStep()
{
}

ReflowStep::ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTemp) : state(state), duration(time), targetTemp(targetTemp), easeFunction(LINEAR) {}

ReflowStep::ReflowStep(ReflowProcessState state, uint8_t time, uint8_t targetTemp, ReflowStepEaseFunction fn) : state(state),
                                                                                                                duration(time),
                                                                                                                targetTemp(targetTemp),
                                                                                                                easeFunction(fn)
{
}

float ReflowStep::calcTempAtPercentage(uint8_t startTemp, float percentage)

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
    case SLOW_RAMP_HOLD:

        if (percentage <= 0.75)
        {
            // Ramp up to the target temperature over the first 25% of the time
            return startTemp + (this->targetTemp - startTemp) * (percentage / 0.75);
        }
        else
        {
            // Hold at the target temperature for the remaining 75% of the time
            return this->targetTemp;
        }

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

    case FAST_RAMP_HOLD:
        if (percentage <= 0.25)
        {
            // Ramp up to the target temperature over the first 75% of the time
            return startTemp + (this->targetTemp - startTemp) * (percentage / 0.25);
        }
        else
        {
            // Hold at the target temperature for the remaining 25% of the time
            return this->targetTemp;
        }
    }
}

ReflowProfile::ReflowProfile(ReflowStep steps[5], char name[20])
{

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
}

void ReflowProfile::start()

{
    timer = StopWatch(StopWatch::MILLIS);
    timer.start();
}

void ReflowProfile::calculateValues()

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

    startTemps[0] = 20; // USe ambient temp as the starting temp for the first step

    // We will grab the current PCB temp from the PID as the start temp otherwise the PID will be off
    //startTemps[0] = tempController.getPlateTemperature();

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

ReflowStep ReflowProfile::reflowStep()

{
    if (!timer.isRunning())
    {
        return steps[0];
    }
    return reflowStep(timer.elapsed());
}

ReflowStep ReflowProfile::reflowStep(uint32_t elapsedMS)

{
    if (!timer.isRunning())
    {
        return steps[0];
    }
    return reflowStep(timer.elapsed());
}

float ReflowProfile::getPercentage()

{
    return timer.elapsed() / (double)TOMILLIS(endTimes[4]);
}

float ReflowProfile::getTargetTemp()

{
    if (!timer.isRunning())
    {
        return startTemps[0];
    }
    return getTargetTemp(timer.elapsed());
}

float ReflowProfile::getTargetTemp(uint32_t elapsedMS)

{
    ReflowStep curStep = reflowStep(elapsedMS);

    uint32_t startTimeMS = TOMILLIS(startTimes[STEPINDEX(curStep)]);
    uint32_t relativeElapsedTime = elapsedMS - startTimeMS;

    float stepPercentage = relativeElapsedTime / (double)TOMILLIS(curStep.duration);
    return curStep.calcTempAtPercentage(startTemps[STEPINDEX(curStep)], stepPercentage);
}

float ReflowProfile::getTargetTempFromPercentage(double processPercentage)

{
    return getTargetTemp(TOMILLIS(endTimes[4]) * processPercentage);
}

uint8_t ReflowProfile::getCurrentStepRelativeTime()

{
    uint32_t elapsedMS = timer.elapsed();
    uint32_t startTimeMS = TOMILLIS(startTimes[STEPINDEX(reflowStep())]);
    return (elapsedMS - startTimeMS) / 1000;
}

void ReflowProfile::toBuffer(uint8_t *b)

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

void ReflowProfile::toEEPROM(uint8_t index)

{
    uint8_t b[40];
    toBuffer(b);
    EEPROM.put(index * PROFILE_SERIALIZED_SIZE, b);
}

ReflowProfile ReflowProfile::fromBuffer(const uint8_t *b)

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

ReflowProfile ReflowProfile::fromEEPROM(uint8_t index)

{
    uint8_t b[40];
    EEPROM.get(index * PROFILE_SERIALIZED_SIZE, b);
    return fromBuffer(b);
}