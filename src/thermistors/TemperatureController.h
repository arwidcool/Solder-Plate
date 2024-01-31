#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

#include "Thermistor.h"
#include "globals.h"

extern Thermistor thermistors[6];

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


    bool thermistorIsActive[6];
    uint8_t activeThermistorCount =0 ;
    float weightFactor;



};
#endif // TEMPERATURE_CONTROLLER_H
