#include "./globals.h"
#include "./EEPROMDataManager.h"



WrappedState<ReflowProcessState> reflowProcessState = WrappedState<ReflowProcessState>(INITIALIZING);
AnalogRef analogRef(5.0);

// Calibration data for 100K thermistors ->https://fab.cba.mit.edu/classes/863.18/CBA/people/erik/reference/11_NTC-3950-100K.pdf ->Glass thermistor NTC 3950 100K 
TempCalibration calibration_100K_3950 = {25, 100000, 86, 10324, 169, 1148};
// Initalize the 3950 100K thermistors with ACTUAL reference resistor measurnment(Measured between Left pin and GND when the board is powered off) using the default calibration data for 100K thermistor

// You can also make a custom calibration data for your thermistor and use that instead of the default one pass it as shown below --> keep the naming of the thermistor the same as the one you are replacing
//TempCalibration calibration_10K_3950 = {25, 10000, 86, 1032, 169, 118};
//Thermistor thermistor1(THERMISTOR1_PIN, 100000, calibration_10K_3950,ThermistorZ_Placement::TOP,ThermistorXY_Placement::MIDDLE);


//--------------------------------------------------------------------------------------------------------------------------------------

//This is where you set your potentiometer  values  and positioning for the 6 thermistors

//You can tweak them from the datasheet to best fit your thermistor but we reccoemnd using the default values and setting the potentiometer to these values
// Does not have to be perfect just set it close to this value and record the measured value and put it for the thermistors
//To measure the resistence turn off the controller completley and measure between GND and the left pin of the connector with the thermistor unplugged

//2.5k reference = Best accuracy around 138C
Thermistor thermistor1(THERMISTOR1_PIN, 2500,ThermistorZ_Placement::BOTTOM,ThermistorXY_Placement::RIGHT);
Thermistor thermistor2(THERMISTOR2_PIN, 2500,ThermistorZ_Placement::TOP,ThermistorXY_Placement::MIDDLE);
Thermistor thermistor3(THERMISTOR3_PIN, 2500,ThermistorZ_Placement::TOP,ThermistorXY_Placement::MIDDLE);
//1k reference = Best accuracy around 173c
Thermistor thermistor4(THERMISTOR4_PIN, 1000,ThermistorZ_Placement::TOP,ThermistorXY_Placement::MIDDLE);
//515R reference = Best accuracy around 210C
Thermistor thermistor5(THERMISTOR5_PIN, 515,ThermistorZ_Placement::TOP,ThermistorXY_Placement::MIDDLE);
//9k reference = Best accuracy around 90C -> This thermistor is used for the preheat phase if attached
Thermistor thermistor6(THERMISTOR6_PIN, 9000,ThermistorZ_Placement::TOP,ThermistorXY_Placement::MIDDLE);


//---------------------------------------------------------------------------------------------------------------------------------------





//Put all thermistors in an array
Thermistor thermistors[6] = {thermistor1, thermistor2, thermistor3, thermistor4, thermistor5, thermistor6};




//---------------------------------------------------------------------------------------------------------------------------------------

// These are the reflow profiles that you can choose from, you can add more if you want (up to 5) but you will have to change the nReflowProfiles variable to the number of profiles you have

int nReflowProfiles = 2;

ReflowProfile reflowProfiles[] = {
    // 138c profile Sn42Bi58
    ReflowProfile(new ReflowStep[5]{
                      ReflowStep(ReflowProcessState::PREHEAT, 120, 35, EASE_OUT),
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

//---------------------------------------------------------------------------------------------------------------------------------------

ReflowProfile chosenReflowProfile = reflowProfiles[0];

//Currently unsued -> can be used to calculate current draw of the system along with system voltage and compared to the current sensor and just be a nice sanity check for the system
uint16_t plateResistanceOhm = 0;


EEPROMDataManager eepromDataManager = EEPROMDataManager();

PidControllerData pidControllerData = {0 /*currentTemp*/, 60 /*TargetTemp*/, 255 /*PWM*/};
PidController pidController = PidController(&pidControllerData);
