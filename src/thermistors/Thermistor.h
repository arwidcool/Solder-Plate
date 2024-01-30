#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>
#include <voltageReference/AnalogRef.h>
#include <common.h>



struct Coefficents
{
    float a;
    float b;
    float c;
};

struct TempCalibration
{
    float lowC;
    float lowR;
    float midC;
    float midR;
    float highC;
    float highR;
};

extern TempCalibration calibration_100K_3950;

#define THERMISTOR1_PIN 39
#define THERMISTOR2_PIN 38
#define THERMISTOR3_PIN 37
#define THERMISTOR4_PIN 36
#define THERMISTOR5_PIN 33
#define THERMISTOR6_PIN 32

extern AnalogRef analogRef;

class Thermistor
{
public:
    // Constructor
    Thermistor();

    Thermistor(uint8_t pin, uint16_t resistance, TempCalibration calibration, ThermistorZ_Placement zPlacement1, ThermistorXY_Placement xyPlacment1);

    Thermistor(uint8_t pin, uint16_t resistance, ThermistorZ_Placement zPlacement1, ThermistorXY_Placement xyPlacment1) ;
 
    // Public Methods
    float getTemperature();
    float getResistance();
    void setPotentiometerResistance(uint16_t resistance) { setRes = resistance; };
    uint16_t getPotentiometerResistance() { return setRes; };
    // Public Variables
    void calculateCoefficents(TempCalibration calibration);

    bool isPluggedIn();
    float getTemperatureFast();
    ThermistorXY_Placement xyPlacment;
    ThermistorZ_Placement zPlacement;
    float scalingFactor ;

private:
    const double K = 273.15;
    float sensorResistance;
    uint8_t thermistorPin;
    float setRes;
    Coefficents coefficents;
    float referenceResistance;
    TempCalibration calibration;

};

#endif // THERMISTOR_H
