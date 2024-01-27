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
    
private:
    bool thermistorIsActive[6];
    uint8_t activeThermistorCount =0 ;

};
#endif // TEMPERATURE_CONTROLLER_H
