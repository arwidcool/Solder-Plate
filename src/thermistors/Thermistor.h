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

// Include any necessary libraries here

class Thermistor
{
public:
    // Constructor
    Thermistor();

    Thermistor(uint8_t pin, float resistance, TempCalibration calibration): thermistorPin(pin), setRes(resistance), calibration(calibration) {
        calculateCoefficents(resistance, calibration);
    }

    Thermistor(uint8_t pin, float resistance): thermistorPin(pin), setRes(resistance), calibration(calibration_100K_3950) {
        calculateCoefficents(resistance, calibration);
    }

    // Public Methods
    int getTemperature();
    float getResistance();
    void setPotentiometerResistance(float resistance);

    // Public Variables
    void calculateCoefficents(float resistance, TempCalibration calibration);

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
