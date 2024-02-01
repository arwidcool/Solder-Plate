#include "TemperatureController.h"

TemperatureController::TemperatureController()
{
}

TemperatureController::~TemperatureController()
{
}

/**
 * Checks if the thermistors are plugged in and updates the status of each thermistor.
 * If a thermistor is plugged in, its corresponding flag is set to true and the active thermistor count is incremented.
 * If a thermistor is not plugged in, its corresponding flag is set to false.
 */
void TemperatureController::checkPluggedInThermistors()
{
    activeThermistorCount = 0;
    for (int i = 0; i < 6; i++)
    {
        bool isPluggedIn = thermistors[i].isPluggedIn();

        if (isPluggedIn)
        {
            // We do it before incrementing because the array is zero indexed

            activeThermistorNumbers[activeThermistorCount] = i;
            activeThermistorCount++;
        }
        else
        {
        }

        // debugC("Thermistor ");
        // debugC(i);
        // debugC(" is plugged in: ");
        // debugC(isPluggedIn == 1 ? "true" : "false");
    }

    debugC("Active thermistor count: ");
    debugC(activeThermistorCount);
}

float TemperatureController::getThermistorTempFast(uint8_t thermistorIndex)
{

    return thermistors[thermistorIndex].getTemperatureFast();
}

float TemperatureController::getWeightedAverage(float *values, float *weights, uint8_t length)
{
    float sum = 0.0;
    float weightSum = 0.0;

    for (int i = 0; i < length; i++)
    {
        sum += values[i] * weights[i];
        weightSum += weights[i];
    }

    // Avoid division by zero
    if (weightSum == 0)
    {
        return 0;
    }

    return sum / weightSum;
}

/**
 * Calculates and returns the average temperature of the solder plate based on the active thermistors.
 * The average temperature is calculated by summing the temperature of each active thermistor and dividing by the number of active thermistors.
 * @return The average temperature of the solder plate.
 */
float TemperatureController::getPlateTemperature()

{
    float activeThermistorTemps[activeThermistorCount];
    float activeThermistorWeights[activeThermistorCount];

    for (int i = 0; i < activeThermistorCount; i++)
    {

        activeThermistorTemps[i] = thermistors[activeThermistorNumbers[i]].getTemperature();
        activeThermistorWeights[i] = thermistors[activeThermistorNumbers[i]].getWeightingFactor();
    }

    float averaged = getWeightedAverage(activeThermistorTemps, activeThermistorWeights, activeThermistorCount);

    return averaged;
}
