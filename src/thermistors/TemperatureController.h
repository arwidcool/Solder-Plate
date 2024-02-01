#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

// #include "Thermistor.h"
#include "../globals.h"





class TemperatureController
{
public:
    TemperatureController();
    ~TemperatureController();

    float getPlateTemperature();
    void checkPluggedInThermistors();
    float static getThermistorTempFast(uint8_t thermistorIndex);
    float static getWeightedAverage(float* values, float* weights, uint8_t length);
    
private:


    uint8_t activeThermistorNumbers[6];
    uint8_t activeThermistorCount;
    float weightFactor;




};
#endif // TEMPERATURE_CONTROLLER_H
