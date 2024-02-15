#include "CurrentMonitor.h"

CurrentMonitor::CurrentMonitor()
{


}

CurrentMonitor::~CurrentMonitor()
{
}

int CurrentMonitor::getSystemCurrentMiliVolts()
{
    int current = currentSensor.mA_DC();
    if (current < 0)
    {
        current = abs(current);
    }
    else
    {
    }

    return current;
}

float CurrentMonitor::getSystemCurrentAmps()
{

    int current = getSystemCurrentMiliVolts();
    float currentAmps = current / 1000.0f;
    return trimFloat(currentAmps, 2);
}

/**
 * @brief Trims a floating-point value to the specified precision.
 *
 * This function takes a floating-point value and trims it to the specified precision.
 * The precision is defined as the number of decimal places to keep.
 *
 * @param value The floating-point value to be trimmed.
 * @param precision The number of decimal places to keep.
 * @return The trimmed floating-point value.
 */
float CurrentMonitor::trimFloat(float value, int precision)
{
    float factor = pow(10.0, precision);   // Calculate the factor (10^decimals)
    return round(value * factor) / factor; // Round the value and return
}

uint8_t CurrentMonitor::calculateMaxPwm()
{
    uint8_t maxPwm = 255;
    // Start with 255 duty cycle and ramp up slowly to get the max duty cycle

    Serial.println("Calculating max PWM");

    for (int i = 255; i > 0; i--)
    {
        analogWrite(17, i);
        Serial.println("PWM: " + String(i));
        int current = getSystemCurrentMiliVolts();
        Serial.println("Current: " + String(current));
        float readInputVoltage = analogRef.calculateInputVoltage();
        Serial.println("Voltage: " + String(readInputVoltage));

        if (current > 6000 || readInputVoltage < 22)
        {

            Serial.println("Limit reached at PWM: " + String(i)) + " Current: " + String(current) + " Voltage: " + String(readInputVoltage);
            maxPwm = i;
            break;
        }

        delay(500);
    }

    analogWrite(17, 255);
    return 255-maxPwm;
}
