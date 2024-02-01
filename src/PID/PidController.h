#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <Arduino.h>
#include <ArduPID.h>


struct PidControllerData { double currentTemp; double setPoint; double targetTemp;};

class PidController
{
public:

    PidController(PidControllerData* data);
    void setSetpoint(double setpoint);
    void setInput(double input);
    double* compute();
    void debug();
    void stop();
    void loop();
    void start();

    double* getInput();
    boolean started = false;

private:
    ArduPID controller;
    PidControllerData *data;
    double integral;
    double previousError;


};


#endif // PIDCONTROLLER_H
