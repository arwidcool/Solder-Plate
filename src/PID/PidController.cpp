#include "PidController.h"

PidController::PidController(double *input, double *output, double *setpoint)
{

    kp = 5;
    kd = 10;
    ki = 1;

    controller.begin(input, output, setpoint, kp, ki, kd);
    controller.reverse();
    controller.setOutputLimits(25, 255);
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

void PidController::debug2(const char* extraText)
{

    controller.debug2(extraText,&Serial, " ", PRINT_INPUT |      // Can include or comment out any of these terms to print
                                                  PRINT_OUTPUT | // in the Serial plotter
                                                  PRINT_SETPOINT | PRINT_BIAS | PRINT_P | PRINT_I | PRINT_D);





}

void PidController::stop()
{

    controller.stop();
    controller.reset();
}

void PidController::start()
{
    controller.start();
}
