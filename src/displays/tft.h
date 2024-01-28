#ifndef TFT_Display_h
#define TFT_Display_h

#include <Arduino.h>
#include <Adafruit_ST7789.h> // Include the ST7789 library
#include <reflow.h>

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
    void clear();

private:
    // Add your private members here
    TFT_XY getCenteredTextXY(char *text);
    TFT_XY getRightAlignedTextXY(char *text, uint16_t x, uint16_t y);
    TFT_XY getLeftAlignedTopTextXY(char *text, uint16_t x, uint16_t y);
    void getMaxTempFromProfile(ReflowProfile *profile);

    uint8_t graphHeight = 180;
    uint16_t graphWidth = 256;
    TFT_XY graphXY = {32, 220};
    uint16_t maxTemp = 0;
    uint16_t totalTIme ;
    uint8_t minTemp = 20;

    TFT_XY getXYWithinGraphBounds(uint8_t temp, uint8_t time);

    void drawGraph();
};

#endif // TFT_H
