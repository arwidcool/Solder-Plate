#include "Thermistor.h"

Thermistor::Thermistor()
{
}

Thermistor::Thermistor(uint8_t pin, float resistance, TempCalibration calibration)
{

    thermistorPin = pin;
    setRes = resistance;

    calculateCoefficents(resistance, calibration);
}

Thermistor::Thermistor(uint8_t pin, float resistance)
{

    thermistorPin = pin;
    setRes = resistance;

    calculateCoefficents(resistance, calibration_100K_3950);
}

Thermistor::~Thermistor()
{
}

float Thermistor::getTemperature()
{

    // Get an average of 5 readings
    float temp = 0;

    uint8_t samples = 5;
    
    for (int i = 0; i < samples; i++)
    {
        getResistance();
        temp += (int)1 / (coefficents.a + coefficents.b * log(sensorResistance) + coefficents.c * (pow(log(sensorResistance), 3))) - K;
    }

    temp = temp / samples;

    return temp;
}

void Thermistor::calculateCoefficents(float resistance, TempCalibration calibration)
{

    float lowK = calibration.lowC + K;
    float midK = calibration.midC + K;
    float highK = calibration.highC + K;

    float lowLnr = log(calibration.lowR);
    float midLnr = log(calibration.midR);
    float highLnr = log(calibration.highR);

    float ln1 = lowLnr - midLnr;
    float ln2 = lowLnr - highLnr;
    float t1 = (1 / lowK) - (1 / midK);
    float t2 = (1 / lowK) - (1 / highK);

    float c = (t1 - ln1 * t2 / ln2) / ((pow(lowLnr, 3) - pow(midLnr, 3)) - ln1 * (pow(lowLnr, 3) - pow(highLnr, 3)) / ln2);
    float b = (t1 - c * (pow(lowLnr, 3) - pow(midLnr, 3))) / ln1;
    float a = 1 / lowK - c * pow(lowLnr, 3) - b * lowLnr;

    coefficents.a = (a);
    coefficents.b = (b);
    coefficents.c = (c);
}

float Thermistor::getResistance()
{

    float systemVoltage = analogRef.calculateSystemVoltage();

    int raw = analogRead(thermistorPin);

    // Get resistance value
    float buffer = raw * systemVoltage;
    float vOut = (buffer) / 1023;
    // Serial.println(vOut);
    buffer = (systemVoltage / vOut) - 1;
    
    // return the resistence
    sensorResistance = setRes * buffer;

    return sensorResistance;
}
