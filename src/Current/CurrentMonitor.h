#ifndef CURRENT_MONITOR_H
#define CURRENT_MONITOR_H

#include "Arduino.h"
#include "globals.h"

class CurrentMonitor {
public:
    CurrentMonitor();
    ~CurrentMonitor();

    int getSystemCurrentMiliVolts();
    float getSystemCurrentAmps();
    float static trimFloat(float value, int precision);
    uint8_t calculateMaxPwm();

    uint8_t calculatedMaxPwm;

private:
    // Add private members and methods here
};

#endif // CURRENT_MONITOR_H
