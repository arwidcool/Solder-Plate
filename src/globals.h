#ifndef GLOBALS_H
#define GLOBALS_H
#include "./common.h"
#include "./thermistors/Thermistor.h"
#include "./reflow.h"
#include "./EEPROMDataManager.h"
#include "./PID/PidController.h"
#include "./thermistors/ThermistorLookup.h"


//Comment out to enable debug messages
//#define DEBUG 

//For printing debug messages
#ifdef DEBUG
#define debugLine(x)  Serial.println(x)
#define debugC(x) Serial.print(x)
#else
#define debugLine(x)
#define debugC(x)
#endif

#define START_TEMP 20
#define LOW_TEMP_THRESHOLD 102
#define MIDDLE_TEMP_THRESHOLD 120
#define HIGH_TEMP_THRESHOLD 150



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
extern ReflowProfile chosenReflowProfile;
extern uint16_t plateResistanceOhm;
extern int nReflowProfiles;

extern PidControllerData pidControllerData;
extern PidController pidController;
extern EEPROMDataManager eepromDataManager;

extern uint16_t reflow_COLOR;
extern uint16_t preheat_COLOR;
extern uint16_t soak_COLOR;
extern uint16_t cool_COLOR;
extern ThermistorLookup thermistorLookup;






#endif