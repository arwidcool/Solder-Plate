#ifndef GLOBALS_H
#define GLOBALS_H
#include "./common.h"
#include "./thermistors/Thermistor.h"
#include "./reflow.h"

extern WrappedState<ReflowProcessState> reflowProcessState;
extern AnalogRef analogRef;
extern Thermistor thermistor1;
extern Thermistor thermistor2;
extern Thermistor thermistor3;
extern Thermistor thermistor4;
extern Thermistor thermistor5;
extern Thermistor thermistor6;
extern ReflowProfile reflowProfiles[];
extern int nReflowProfiles;
#endif