#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>
#include <voltageReference/AnalogRef.h>

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


enum ThermistorZ_Placement
{
    TOP,
    BOTTOM
};

enum ThermistorXY_Placement
{
    MIDDLE,
    LEFT,
    RIGHT
};

class Thermistor
{
public:
    // Constructor
    Thermistor();

    Thermistor(uint8_t pin, uint16_t resistance, TempCalibration calibration,ThermistorZ_Placement zPlacement,ThermistorXY_Placement  xyPlacment) : thermistorPin(pin), setRes(resistance), calibration(calibration),zPlacement(zPlacement),xyPlacment(xyPlacment)
    {
        calculateCoefficents(calibration);
        calculateScalingFactor();
    }

    Thermistor(uint8_t pin, uint16_t resistance,ThermistorZ_Placement zPlacement,ThermistorXY_Placement  xyPlacment) : thermistorPin(pin), setRes(resistance), calibration(calibration_100K_3950),zPlacement(zPlacement),xyPlacment(xyPlacment)
    {
        calculateCoefficents(calibration);
        calculateScalingFactor();
    }

    // Public Methods
    float getTemperature();
    float getResistance();
    void setPotentiometerResistance(uint16_t resistance) { setRes = resistance; };
    uint16_t getPotentiometerResistance() { return setRes; };
    // Public Variables
    void calculateCoefficents(TempCalibration calibration);
    float getScalingFactor() { return scalingFactor; };

    bool isPluggedIn();

private:
    ThermistorZ_Placement zPlacement;
    ThermistorXY_Placement xyPlacment;
    const double K = 273.15;
    float sensorResistance;
    uint8_t thermistorPin;
    uint16_t setRes;
    Coefficents coefficents;
    float referenceResistance;
    TempCalibration calibration;
    float scalingFactor ;
    void calculateScalingFactor();


};

#endif // THERMISTOR_H
