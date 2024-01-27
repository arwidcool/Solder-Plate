#ifndef EXECUTIONTIMER_H
#define EXECUTIONTIMER_H

#include <Arduino.h>


class ExecutionTimer {
private:
    unsigned long startTime;
    unsigned long endTime;
    bool isRunning;

public:
    ExecutionTimer() {
        startTime = 0;
        endTime = 0;
        isRunning = false;
    }

    void start() {
        startTime = millis();  // Use micros() for microsecond precision
        isRunning = true;
    }

    unsigned long stop() {
        if (isRunning) {
            endTime = millis();  // Use micros() for microsecond precision
            isRunning = false;

            Serial.print("Execution time: ");
            Serial.print(endTime - startTime);
            Serial.println(" ms");
            return endTime - startTime;
        } else {
            return 0; // or retain the last execution time
        }
    }

    // Optional: Function to get the elapsed time without stopping
    unsigned long elapsed() const {
        if (isRunning) {
            return millis() - startTime;  // Use micros() for microsecond precision
        } else {
            return endTime - startTime;
        }
    }
};

#endif // EXECUTIONTIMER_H