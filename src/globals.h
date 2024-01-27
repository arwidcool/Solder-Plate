#ifndef GLOBALS_H
#define GLOBALS_H
#include "./common.h"
#include "./thermistors/Thermistor.h"
#include "./reflow.h"
#include "./EEPROMDataManager.h"

extern WrappedState<ReflowProcessState> reflowProcessState;
extern AnalogRef analogRef;
extern Thermistor thermistor1;
extern Thermistor thermistor2;
extern Thermistor thermistor3;
extern Thermistor thermistor4;
extern Thermistor thermistor5;
extern Thermistor thermistor6;
extern Thermistor thermistors[6];
extern ReflowProfile reflowProfiles[];
extern uint16_t plateResistanceOhm;
// EEPROM data manager is in its own file
extern int nReflowProfiles;


extern EEPROMDataManager eepromDataManager;

#endif