#ifndef EEPROMDATAMANAGER_H
#define EEPROMDATAMANAGER_H
#include <Arduino.h>
#define EEPROM_START_TERMISTORS 2
#define EEPROM_START_PLATERES (EEPROM_START_TERMISTORS + 2 * 6)
#define EEPROM_START_REFLOWPROFILES (EEPROM_START_PLATERES + 2)

class EEPROMDataManager
{
public:
    EEPROMDataManager(){};
    void setup();
    void saveToEEPROM();
};

#endif