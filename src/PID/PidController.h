#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <Arduino.h>
#include <ArduPID.h>

class PidController
{
public:

    PidController(double* input, double* output, double* setpoint);
    void setSetpoint(double setpoint);
    void setInput(double input);
    double* compute();
    void debug();
    void debug2(const char* extraText);
    void stop();

    boolean started = false;

private:
    ArduPID controller;
    double kp;
    double ki;
    double kd;
    double integral;
    double previousError;


};

#endif // PIDCONTROLLER_H
