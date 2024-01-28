#ifndef TFT_Display_h
#define TFT_Display_h

#include <Arduino.h>
#include <Adafruit_ST7789.h> // Include the ST7789 library
#include <reflow.h>
#include <globals.h>
#include <StopWatch.h>

struct TFT_XY
{
    int x;
    int y;
};

class TFT_Display
{
public:
    TFT_Display();
    ~TFT_Display();

    void start();
    // Add your class methods here
    void init(ReflowProfile *profile);
    void drawRealTemp(double *temp, float time);
    void clear();

private:

    TFT_XY prevousTempXY = {32, 220};

    StopWatch drawTimer;

    ReflowProfile *profile;

    uint8_t tickMarkLength = 5;
    // Add your private members here
    TFT_XY getCenteredTextXY(char *text);
    TFT_XY getRightAlignedTextXY(char *text, uint16_t x, uint16_t y);
    TFT_XY getLeftAlignedTopTextXY(char *text, uint16_t x, uint16_t y);
    TFT_XY getCenterAlignedBottomTextXY(char *text, uint16_t x, uint16_t y);
    void getMaxTempFromProfile(ReflowProfile *profile);
    void getTotalTimeFromProfile(ReflowProfile *profile);

    uint16_t graphHeight = 180;
    uint16_t graphWidth = 255;
    TFT_XY graphXY = {32, 220};
    uint16_t maxTemp = 0;
    uint16_t totalTIme =0;
    uint8_t minTemp = 20;

    TFT_XY getXYWithinGraphBounds(uint8_t temp, uint8_t time);

    void drawGraph();
    void drawGraphAxis();
    void drawGraphAxisLabels();
    void drawGraphAxisTicks();
    void drawGraphAxisTickLabels();
    void drawGraphReflowStagesBackground();
    void drawReflowTargetTempLine();

    uint16_t tempYonGraph(float temp);
    uint16_t tempYonGraph(float *temp);
    uint16_t timeXonGraph(float time);

    template <typename T>
    char *numberToCharPtr(T number);
};

#endif // TFT_H

/**
 * Converts a number to a character pointer.!! remember to delete the pointer after use
 *
 * @tparam T The type of the number.
 * @param number The number to be converted.
 * @return A dynamically allocated character pointer containing the converted number.
 */
template <typename T>
inline char *TFT_Display::numberToCharPtr(T number)
{
    String numStr = String(number);
    char *charPtr = new char[numStr.length() + 1];
    strcpy(charPtr, numStr.c_str());
    return charPtr;
}
