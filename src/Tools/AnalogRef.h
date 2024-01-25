
#ifndef ANALOGREF_H
#define ANALOGREF_H

#include <Arduino.h>

class AnalogRef
{
private:
  float systemVoltage; // Reference voltage in volts

  boolean systemVolatgeBelow4_5V = false;
  uint8_t voltageRefPin = 27;

  uint8_t voltageInPin = 28;

public:
  float sysVoltage;
  float sysMultiplyer;
  float outputVoltage;
  float outputVoltageMultiplyer;
  float differenceMultiplyer;
  AnalogRef(float systemVoltage)
  {
    this->systemVoltage = systemVoltage;
  }

  float calculateSystemVoltage()
  {
    const uint8_t sampleCount = 1;

    uint16_t rawValue = 0;

    for (size_t i = 0; i < sampleCount; ++i)
    {
      rawValue += analogRead(voltageRefPin);
    }

    const float voltageRef = 1023.0;
    const float systemVoltage = 5.0;
    const float systemVoltageOut = rawValue / sampleCount * systemVoltage / voltageRef;
    const float voltage5V = 2.5 / systemVoltageOut * systemVoltage;

    systemVolatgeBelow4_5V = voltage5V < 4.5;

    return voltage5V;
  }

  float calculateSystemVoltageMultyplyer()
  {

    // Calculate the actual 5V system voltage
    float currentSystemVoltage = calculateSystemVoltage();

    // Calculate the actual 5V system voltage
    float voltage_5V_multiplyer = (5 / currentSystemVoltage);

    return voltage_5V_multiplyer;
  }

  float calculateInputVoltage()
  {

    calculateSystemVoltage();

    float value;

    uint8_t sampleCount = 20;
    value = 0;

    for (size_t i = 0; i < sampleCount; ++i)
    {
      value += analogRead(voltageInPin);
    }

    value /= sampleCount;

    float vOut = (value * systemVoltage) / 1024.0;

    return vOut * 5.8;
  }

  void calculate()
  {

    sysVoltage = calculateSystemVoltage();
    sysMultiplyer = 5 / sysVoltage;
  }
};

#endif // ANALOGREF_H
