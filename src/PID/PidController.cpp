#include "PidController.h"
#include "globals.h"
#define MOSTFET_PIN 17
#include "voltageReference/AnalogRef.h"

extern AnalogRef analogRef;

PidController::PidController(PidControllerData *data)
{

    this->data = data;
    kp = 5;
    kd = 10;
    ki = 1;

    controller.begin(&(this->data->currentTemp), &(this->data->setPoint), &(this->data->targetTemp), kp, ki, kd);
    controller.reverse();
    controller.setOutputLimits(0, 255);
    //controller.setSampleTime(20);
    controller.setWindUpLimits(-100, 185);
}

double *PidController::compute()
{

    controller.compute();

    return controller.output;
}

void PidController::debug()
{

    controller.debug(&Serial, " ", PRINT_INPUT |      // Can include or comment out any of these terms to print
                                       PRINT_OUTPUT | // in the Serial plotter
                                       PRINT_SETPOINT | PRINT_BIAS | PRINT_P | PRINT_I | PRINT_D);
}

void PidController::loop()
{
    data->targetTemp = chosenReflowProfile.getTargetTemp();
    
    data->currentTemp = thermistor1.getTemperature();

    float sysVoltage = analogRef.calculateSystemVoltage();
    compute();
    analogWrite(MOSTFET_PIN, data->setPoint);
}

void PidController::stop()
{
    data->targetTemp = 0; // should not be needed but why not? --- Its needed it randomly starts again and sometimes goes to heat the plate
                          // STOP --> Always use analog write for sanity- Keep PWM pins labled analogWrite -- Writing it LOW will KEEP IT ON

    analogWrite(MOSTFET_PIN, 255); // VERY IMPORTANT, DONT CHANGE!
    controller.reset();
    controller.stop();


}

void PidController::start()
{
    controller.start();
    
}

double* PidController::getInput()
{
    return controller.input;
}
