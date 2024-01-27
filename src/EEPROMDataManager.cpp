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

    for (int i = 0; i < 6; i++)
    {
        uint16_t resUint;
        EEPROM.get(2 * i + EEPROM_START_TERMISTORS, resUint);
        float res = resUint / 1000.0;
        thermistors[i].setPotentiometerResistance(res);
    }

    EEPROM.get(EEPROM_START_PLATERES, plateResistanceOhm);
}
void EEPROMDataManager::saveToEEPROM()
{
    EEPROM.begin();
    uint16_t tmp = 0x1234;
    EEPROM.put(0, &tmp); // initialize the EEPROM

    for (int i = 0; i < 6; i++)
    {
        float res = thermistors[i].getResistance();
        uint16_t resUint = res * 1000;
        EEPROM.put(2 * i + EEPROM_START_TERMISTORS, &resUint);
    }

    EEPROM.put(EEPROM_START_PLATERES, &plateResistanceOhm);
}