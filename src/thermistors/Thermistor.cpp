#include "Thermistor.h"

/**
 * @brief Calculates and returns the temperature based on the resistance of the thermistor.
 *
 * This function calculates the temperature using the Steinhart-Hart equation and the coefficients
 * provided. It takes an average of 5 resistance readings and converts it to temperature using the
 * scaling factor. The calculated temperature is returned.
 *
 * @return The temperature in degrees Celsius.
 */
float Thermistor::getTemperature()
{

    // Get an average of 5 readings
    float temp = 0;

    uint8_t samples = 5;

    for (int i = 0; i < samples; i++)
    {
        getResistance();
        temp += (int)1 / (coefficents.a + coefficents.b * log(sensorResistance) + coefficents.c * (pow(log(sensorResistance), 3))) - K;
    }

    temp = temp / samples;

    // The scaling factor should only be applied when the plate is being heated up -> 60C seems like a good threshold unless you live in the sahara desert with no AC
    // Its non-linear so it will be more accurate so we will probably need to impliment a refrence table for the scaling factor this is just a rough estimate it will be based on a sensor calibrated on the top middle of the plate
    
    //TODO: Impliment a method to calculate the scaling factor based on the placement of the thermistor by measurnment sof refrence center thermistor to other positions
    if (temp > 60)
    {
        temp = temp * scalingFactor;
    }

    return temp;
}

/**
 * Calculates the coefficients for the thermistor based on the given temperature calibration.
 *
 * @param calibration The temperature calibration data.
 */
void Thermistor::calculateCoefficents(TempCalibration calibration)
{

    float lowK = calibration.lowC + K;
    float midK = calibration.midC + K;
    float highK = calibration.highC + K;

    float lowLnr = log(calibration.lowR);
    float midLnr = log(calibration.midR);
    float highLnr = log(calibration.highR);

    float ln1 = lowLnr - midLnr;
    float ln2 = lowLnr - highLnr;
    float t1 = (1 / lowK) - (1 / midK);
    float t2 = (1 / lowK) - (1 / highK);

    float c = (t1 - ln1 * t2 / ln2) / ((pow(lowLnr, 3) - pow(midLnr, 3)) - ln1 * (pow(lowLnr, 3) - pow(highLnr, 3)) / ln2);
    float b = (t1 - c * (pow(lowLnr, 3) - pow(midLnr, 3))) / ln1;
    float a = 1 / lowK - c * pow(lowLnr, 3) - b * lowLnr;

    coefficents.a = (a);
    coefficents.b = (b);
    coefficents.c = (c);
}

bool Thermistor::isPluggedIn()
{
    // check if the resistnece is INF is so then the thermistor is not plugged in
    if (getResistance() == INFINITY)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * Calculates the scaling factor for the thermistor based on its placement in the 3D space.
 * The scaling factor is used to adjust the temperature readings of the thermistor.
 */
void Thermistor::calculateScalingFactor()
{

    switch (zPlacement)
    {
    case TOP:
        switch (xyPlacment)
        {
        case MIDDLE:
            scalingFactor = 1;
            break;
        case LEFT:
            scalingFactor = 1.1;
            break;
        case RIGHT:
            scalingFactor = 1.1;
            break;
        }
        break;

    case BOTTOM:
        switch (xyPlacment)
        {
        case MIDDLE:
            scalingFactor = 1.1;
            break;
        case LEFT:
            scalingFactor = 1.2;
            break;
        case RIGHT:
            scalingFactor = 1.2;
            break;
        }
        break;
    }
}

float Thermistor::getTemperatureFast()
{

    // Get only one reading
    getResistance();

    float temp = (int)1 / (coefficents.a + coefficents.b * log(sensorResistance) + coefficents.c * (pow(log(sensorResistance), 3))) - K;

    // The scaling factor should only be applied when the plate is being heated up -> 60C seems like a good threshold unless you live in the sahara desert with no AC

    // Its non-linear so it will be more accurate so we will probably need to impliment a refrence table for the scaling factor this is just a rough estimate it will be based on a sensor calibrated on the top middle of the plate

    if (temp > 60)
    {
        temp = temp * scalingFactor;
    }

    return temp;
}

float Thermistor::getResistance()
{

    float systemVoltage = analogRef.calculateSystemVoltage();

    // int raw = analogRead(thermistorPin);

    // // Get resistance value
    // float buffer = raw * systemVoltage;
    // float vOut = (buffer) / 1023;

    // // Calculate the resistance of the thermistor with the system voltage accounted for
    // buffer = (systemVoltage / vOut) - 1;

    // // return the resistence
    // sensorResistance = setRes * buffer;

    int sensorValue = analogRead(thermistorPin);                      // Read the analog value (0-1023)
    float voltage = sensorValue * (systemVoltage / 1023.0);           // Convert to voltage
    float R_unknown = (setRes * (systemVoltage - voltage)) / voltage; // Calculate the unknown resistor's value
    sensorResistance = R_unknown;

    return sensorResistance;
}
