#include "Thermistor.h"
#include "../globals.h"

/**
 * @brief Calculates and returns the temperature based on the resistance of the thermistor.
 *
 * This function calculates the temperature using the Steinhart-Hart equation and the coefficients
 * provided. It takes an average of 5 resistance readings and converts it to temperature using the
 * scaling factor. The calculated temperature is returned.
 *
 * @return The temperature in degrees Celsius.
 */
Thermistor::Thermistor()
{
}
Thermistor::Thermistor(uint8_t pin, uint16_t resistance, TempCalibration calibration, ThermistorZ_Placement zPlacement1, ThermistorXY_Placement xyPlacment1) : thermistorPin(pin), setRes(resistance), calibration(calibration)
{
    calculateCoefficents(calibration);

    zPlacement = zPlacement1;

    xyPlacment = xyPlacment1;
}
Thermistor::Thermistor(uint8_t pin, uint16_t resistance, ThermistorZ_Placement zPlacement1, ThermistorXY_Placement xyPlacment1) : thermistorPin(pin), setRes(resistance), calibration(calibration_100K_3950)
{

    calculateCoefficents(calibration);

    zPlacement = zPlacement1;

    xyPlacment = xyPlacment1;
}
float Thermistor::getTemperature()
{
    scalingFactor = 1;
    // Get an average of 5 readings
    float temp = 0;

    uint8_t samples = 5;

    for (int i = 0; i < samples; i++)
    {
        getResistance();
        temp += (int)1 / (coefficents.a + coefficents.b * log(sensorResistance) + coefficents.c * (pow(log(sensorResistance), 3))) - K;
    }

    temp = temp / samples;

    scalingFactor = thermistorLookup.getFactor(zPlacement, xyPlacment, temp);
    temp = temp * scalingFactor;

    weightFactor = getWeightingFactor();

    currenTemperature = temp;

    return temp;
}

/**
 * @brief Returns the weighting factor for the thermistor based on its most accurate temperature range  and current temp.
 * The range is based on reference resistor values.
 * This should only be called after getTemperature() has been called.
 * @return The scaling factor.
 */
float Thermistor::getWeightingFactor()
{
    switch (xyPlacment)
    {

    // Middle always have weighting factor of 1 and the factor can only increase for the other sensors
    case MIDDLE:

        return 1;
        break;

    case MIDDLE_LOW_TEMP:

        if (currenTemperature > START_TEMP && currenTemperature < LOW_TEMP_THRESHOLD)
        {
            return ThermistorLookup::interpolate(currenTemperature, START_TEMP, LOW_TEMP_THRESHOLD, 1, 1.5);
        }

        else if (currenTemperature > LOW_TEMP_THRESHOLD && currenTemperature < MIDDLE_TEMP_THRESHOLD)
        {
            return ThermistorLookup::interpolate(currenTemperature, LOW_TEMP_THRESHOLD, MIDDLE_TEMP_THRESHOLD, 1.5, 1);
        }

        else if (currenTemperature > MIDDLE_TEMP_THRESHOLD && currenTemperature < HIGH_TEMP_THRESHOLD)
        {
            return ThermistorLookup::interpolate(currenTemperature, MIDDLE_TEMP_THRESHOLD, HIGH_TEMP_THRESHOLD, 1, 0.1);
        }
        else if (currenTemperature < START_TEMP)
        {
            return 1;
        }
        else
        {
            return 0.1;
        }


        case MIDDLE_HIGH_TEMP:

            if (currenTemperature > MIDDLE_TEMP_THRESHOLD && currenTemperature < HIGH_TEMP_THRESHOLD)
            {
                return ThermistorLookup::interpolate(currenTemperature, MIDDLE_TEMP_THRESHOLD, HIGH_TEMP_THRESHOLD, 1, 1.5);
            }

            else if (currenTemperature > HIGH_TEMP_THRESHOLD)
            {
                return 1.5;
            }
            else if (currenTemperature < MIDDLE_TEMP_THRESHOLD)
            {
                return ThermistorLookup::interpolate(currenTemperature, START_TEMP, MIDDLE_TEMP_THRESHOLD, 0.1, 1);
            }
            else
            {
                return 0.1;
            }
            break;

        default:
            return 1;
            break;
        }
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

    float Thermistor::getTemperatureFast()
    {

        // Get only one reading
        getResistance();

        float temp = (int)1 / (coefficents.a + coefficents.b * log(sensorResistance) + coefficents.c * (pow(log(sensorResistance), 3))) - K;

        // The scaling factor should only be applied when the plate is being heated up -> 60C seems like a good threshold unless you live in the sahara desert with no AC

        // Its non-linear so it will be more accurate so we will probably need to impliment a refrence table for the scaling factor this is just a rough estimate it will be based on a sensor calibrated on the top middle of the plate

        // float scalingFactor = ThermistorLookup::getFactor(thermistorPin, temp);
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
