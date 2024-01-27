#include "PidController.h"
#include "globals.h"
#define MOSTFET_PIN 17

PidController::PidController(PidControllerData *data)
{

    this->data = data;
    kp = 5;
    kd = 10;
    ki = 1;

    controller.begin(&(this->data->currentTemp), &(this->data->setPoint), &(this->data->targetTemp), kp, ki, kd);
    controller.reverse();
    controller.setOutputLimits(0, 255);
    controller.setSampleTime(20);
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

void PidController::loop() {
    data->targetTemp = chosenReflowProfile.getTargetTemp();
    data->currentTemp = thermistor1.getTemperature();
    compute();
    analogWrite(MOSTFET_PIN, data->setPoint);
}

void PidController::stop()
{
    data->targetTemp = 0; // should not be needed but why not?
    // STOP
    digitalWrite(MOSTFET_PIN, LOW);
    controller.stop();
    controller.reset();
}

void PidController::start()
{
    controller.start();
}
