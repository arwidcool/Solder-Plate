#include "./globals.h"
#include "./EEPROMDataManager.h"

WrappedState<ReflowProcessState> reflowProcessState = WrappedState<ReflowProcessState>(INITIALIZING);
AnalogRef analogRef(5.0);

// Calibration data for 100K thermistors ->https://fab.cba.mit.edu/classes/863.18/CBA/people/erik/reference/11_NTC-3950-100K.pdf ->Glass thermistor NTC 3950 100K 
TempCalibration calibration_100K_3950 = {25, 100000, 86, 10324, 169, 1148};
// Initalize the 3950 100K thermistors with ACTUAL reference resistor measurnment(Measured between Left pin and GND when the board is powered off) using the default calibration data for 100K thermistor

//2.5k reference = Best accuracy around 138C
Thermistor thermistor1(THERMISTOR1_PIN, 2500);
Thermistor thermistor2(THERMISTOR2_PIN, 2500);
Thermistor thermistor3(THERMISTOR3_PIN, 2500);
//1k reference = Best accuracy around 173c
Thermistor thermistor4(THERMISTOR4_PIN, 1000);
//515R reference = Best accuracy around 210C
Thermistor thermistor5(THERMISTOR5_PIN, 515);
//9k reference = Best accuracy around 90C
Thermistor thermistor6(THERMISTOR6_PIN, 9000);
//Put all thermistors in an array
Thermistor thermistors[6] = {thermistor1, thermistor2, thermistor3, thermistor4, thermistor5, thermistor6};

ReflowProfile reflowProfiles[] = {
    // 138c profile Sn42Bi58
    ReflowProfile(new ReflowStep[5]{
                      ReflowStep(ReflowProcessState::PREHEAT, 60, 100, EASE_OUT),
                      ReflowStep(ReflowProcessState::SOAK, 90, 155),
                      ReflowStep(ReflowProcessState::REFLOW, 45, 185, EASE_OUT),
                      ReflowStep(ReflowProcessState::COOL, 45, 155, EASE_IN),
                      ReflowStep(ReflowProcessState::DONE, 0, 0)},
                    "138c Sn42Bi58\0"),
                  
    ReflowProfile(new ReflowStep[5]{
                     ReflowStep(ReflowProcessState::PREHEAT, 100, 150),
                     ReflowStep(ReflowProcessState::SOAK, 30, 180),
                     ReflowStep(ReflowProcessState::REFLOW, 80, 220, EASE_IN_OUT),
                     ReflowStep(ReflowProcessState::COOL, 30, 183),
                     ReflowStep(ReflowProcessState::DONE, 0, 0)}, 
                    "183C Sn63 Pb37 \0"),

 

};

ReflowProfile chosenReflowProfile = reflowProfiles[0];

int nReflowProfiles = 2;

uint16_t plateResistanceOhm = 0;
EEPROMDataManager eepromDataManager = EEPROMDataManager();

PidControllerData pidControllerData = {0 /*currentTemp*/, 60 /*TargetTemp*/, 255 /*PWM*/};
PidController pidController = PidController(&pidControllerData);
