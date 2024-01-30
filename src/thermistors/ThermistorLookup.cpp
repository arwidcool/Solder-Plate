#include "ThermistorLookup.h"

ThermistorLookup::ThermistorLookup()
{
}

ThermistorLookup::~ThermistorLookup()
{
}

float ThermistorLookup::getFactor(ThermistorZ_Placement zPlacement, ThermistorXY_Placement xyPlacment, uint8_t temperature)
{
    const LookupEntry *lookupTable = getTable(zPlacement, xyPlacment);
    int numEntries = tableSize;

    if (lookupTable == noScaling)
    {
        return 1;
    }
    else
    {
        // Handle boundary cases
        if (temperature <= lookupTable[0].temperature)
        {
            return lookupTable[0].value;
        }
        if (temperature >= lookupTable[numEntries - 1].temperature)
        {
            return lookupTable[numEntries - 1].value;
        }

        // Find the interval containing the temperature
        for (int i = 0; i < numEntries - 1; ++i)
        {
            if (temperature >= lookupTable[i].temperature && temperature < lookupTable[i + 1].temperature)
            {
                // Perform linear interpolation
                uint8_t t0 = lookupTable[i].temperature;
                float v0 = lookupTable[i].value;
                uint8_t t1 = lookupTable[i + 1].temperature;
                float v1 = lookupTable[i + 1].value;

                float slope = (v1 - v0) / (t1 - t0);
                return v0 + slope * (temperature - t0);
            }
        }
    }
    // This point should never be reached
    return 1;
}

const LookupEntry *ThermistorLookup::getTable(ThermistorZ_Placement zPlacement, ThermistorXY_Placement xyPlacment)
{
    tableSize = 0;
    switch (zPlacement)
    {
    case TOP:
        switch (xyPlacment)
        {
        case MIDDLE:

            tableSize = sizeof(noScaling) / sizeof(LookupEntry);
            return noScaling;
        case SIDE:

            tableSize = sizeof(lookupTopSide) / sizeof(LookupEntry);
            return lookupTopSide;
        }
    case BOTTOM:

        switch (xyPlacment)
        {
        case MIDDLE:

            tableSize = sizeof(lookupBottomMiddle) / sizeof(LookupEntry);
            return lookupBottomMiddle;
        case SIDE:

            tableSize = sizeof(lookupTopSide) / sizeof(LookupEntry);
            return lookupTopSide;
        }
    }
}

float ThermistorLookup::interpolate(float x, float x0, float x1, float y0, float y1)
{
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}
