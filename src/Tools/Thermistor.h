#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>
#include <Tools/AnalogRef.h>


#define THERMISTOR1_PIN 39
#define THERMISTOR2_PIN 38
#define THERMISTOR3_PIN 37
#define THERMISTOR4_PIN 36
#define THERMISTOR5_PIN 33
#define THERMISTOR6_PIN 32

extern AnalogRef analogRef;

// Include any necessary libraries here

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

class Thermistor
{
public:
    // Constructor
    Thermistor();

    Thermistor(uint8_t pin, float resistance, TempCalibration calibration);

    // Destructor
    ~Thermistor();

    // Public Methods
    int getTemperature();
        float getResistance();

    // Public Variables

private:

    const double K = 273.15;

    float sensorResistance;

    uint8_t thermistorPin;
    float setRes;

    Coefficents coefficents;

    float referenceResistance;

    void calculateCoefficents(float resistance, TempCalibration calibration);




};

#endif // THERMISTOR_H
