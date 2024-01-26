
#ifndef ANALOGREF_H
#define ANALOGREF_H

#include <Arduino.h>

class AnalogRef
{
private:
  float systemVoltage; // Reference voltage in volts

  uint8_t voltageRefPin = 27;
  uint8_t voltageInPin = 28;

public:
  float sysVoltage;
  float sysMultiplyer;
  float inputVoltage;

  //Set what the system voltage SHOULD be
  AnalogRef(float systemVoltage)
  {
    this->systemVoltage = systemVoltage;
  }


  //Calculate the system voltage
  float calculateSystemVoltage()
  {
    const uint8_t SAMPLE_COUNT = 1;
    const float VOLTAGE_REF = 1023.0;
    uint16_t rawValue = 0;
    for (size_t i = 0; i < SAMPLE_COUNT; ++i)
    {
      rawValue += analogRead(voltageRefPin);
    }

    float meanRawValue = (float)rawValue / SAMPLE_COUNT;
    float outputVoltage = meanRawValue / VOLTAGE_REF * systemVoltage;
    float voltage5V = 2.5 / outputVoltage * systemVoltage;

    return voltage5V;
  }

  //Used for external voltage readings not powered by the 5V system
  float calculateSystemVoltageMultyplyer()
  {

    // Calculate the actual 5V system voltage
    float currentSystemVoltage = calculateSystemVoltage();

    // Calculate the actual 5V system voltage
    float voltage_5V_multiplyer = (5 / currentSystemVoltage);

    return voltage_5V_multiplyer;
  }


  //Calculate the input voltage of the power supply
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


};

#endif // ANALOGREF_H
