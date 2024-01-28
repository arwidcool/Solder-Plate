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
    for (int i = 0; i < 6; i++)
    {
        bool isPluggedIn = thermistors[i].isPluggedIn();

        if (isPluggedIn)
        {
            thermistorIsActive[i] = true;
            activeThermistorCount++;
        }
        else
        {
            thermistorIsActive[i] = false;
        }

        debugC("Thermistor ");
        debugC(i);
        debugC(" is plugged in: ");
        debugC(isPluggedIn == 1 ? "true" : "false");
    }

    debugC("Active thermistor count: ");
    debugLine(activeThermistorCount);
}

float TemperatureController::getThermistorTempFast(uint8_t thermistorIndex)
{
    
    return thermistors[thermistorIndex].getTemperatureFast();
}



/**
 * Calculates and returns the average temperature of the solder plate based on the active thermistors.
 * The average temperature is calculated by summing the temperature of each active thermistor and dividing by the number of active thermistors.
 * @return The average temperature of the solder plate.
 */
float TemperatureController::getPlateTemperature()
{
    float plateAveragedTemp = 0;

    for (int i = 0; i < 6; i++)
    {
        if (thermistorIsActive[i])
        {
            plateAveragedTemp += thermistors[i].getTemperature();
        }
    }

    return plateAveragedTemp / activeThermistorCount;
}
