#include "EEPROMDataManager.h"
#include "globals.h"

void EEPROMDataManager::setup()
{
    EEPROM.begin();
    uint16_t eepromData;
    EEPROM.get(0, eepromData);
    if (eepromData != 0x1234)
    {
        // EEPROM is empty, so we need to initialize it
        saveToEEPROM();
    }

    // THermistors part. for now we let the user set the thermistor values in code.

    // for (int i = 0; i < 6; i++)
    // {
    //     uint16_t resUint;
    //     EEPROM.get(2 * i + EEPROM_START_TERMISTORS, resUint);
    //     thermistors[i].setPotentiometerResistance(resUint);
    // }

    EEPROM.get(EEPROM_START_PLATERES, plateResistanceOhm);
}
void EEPROMDataManager::saveToEEPROM()
{
    EEPROM.begin();
    uint16_t tmp = 0x1234;
    EEPROM.put(0, tmp); // initialize the EEPROM

    // THermistors part. for now we dont save the thermistor values in code.

    // for (int i = 0; i < 6; i++)
    // {
    //     uint16_t res = thermistors[i].getPotentiometerResistance();
    //     EEPROM.put(2 * i + EEPROM_START_TERMISTORS, res);
    // }

    EEPROM.put(EEPROM_START_PLATERES, plateResistanceOhm);
}