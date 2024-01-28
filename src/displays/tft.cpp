#include "tft.h"

// LCD display pins
#define TFT_CS 7
#define TFT_RST 12
#define TFT_DC 14
#define MOSI 4 // MOSI
#define SCK 6  // SCK
// Create an instance of the Adafruit ST7789 class using the custom SPI pins

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, MOSI, SCK, TFT_RST);

TFT_Display::TFT_Display()
{
}

TFT_Display::~TFT_Display()
{
}

void TFT_Display::start()
{
    tft.init(240, 320); // Init ST7789 320x240
    tft.setRotation(1); // Set screen orientation
}

void TFT_Display::init(ReflowProfile *profile)
{

    clear();

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    TFT_XY xy = getCenteredTextXY(profile->name);
    tft.setCursor(xy.x, xy.y);
    tft.println(profile->name);
    getMaxTempFromProfile(profile);
    getTotalTimeFromProfile(profile);

    drawGraph();
}

void TFT_Display::clear()
{
    tft.fillScreen(ST77XX_BLACK);
}

TFT_XY TFT_Display::getCenteredTextXY(char *text)
{
    TFT_XY xy;

    uint8_t textLength = strlen(text);
    int16_t x, y;
    uint16_t w, h;
    w = tft.width();
    h = tft.height();

    tft.getTextBounds(text, 0, 0, &x, &y, &w, &h);

    xy.x = (w - textLength) / 2;
    xy.y = (h - 1) / 2;

    return xy;
}

TFT_XY TFT_Display::getRightAlignedTextXY(char *text, uint16_t x, uint16_t y)
{
    TFT_XY xy;

    uint8_t textLength = strlen(text) * 6;
    uint8_t textHeight = 4;
    int16_t x1, y1;
    uint16_t w, h;
    w = tft.width();
    h = tft.height();

    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    xy.x = x - textLength - 1;
    xy.y = y - textHeight;

    return xy;
}

TFT_XY TFT_Display::getLeftAlignedTopTextXY(char *text, uint16_t x, uint16_t y)
{
    TFT_XY xy;

    uint8_t textLength = strlen(text);
    int16_t x1, y1;
    uint16_t w, h;
    w = tft.width();
    h = tft.height();

    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    xy.x = x;
    xy.y = y - h - 1;

    return xy;
}

TFT_XY TFT_Display::getCenterAlignedBottomTextXY(char *text, uint16_t x, uint16_t y)
{
    TFT_XY xy;

    uint8_t textLength = strlen(text);
    int16_t x1, y1;
    uint16_t w, h;
    w = tft.width();
    h = tft.height();

    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    xy.x = x - (w / 2);
    xy.y = y;

    return xy;
}

void TFT_Display::getMaxTempFromProfile(ReflowProfile *profile)
{

    for (uint8_t i = 0; i < 5; i++)
    {
        if (profile->steps[i].targetTempAtEnd > maxTemp)
        {
            maxTemp = profile->steps[i].targetTempAtEnd;
        }
    }

    if (maxTemp < 200)
    {
        maxTemp = 200;
    }
    else if (maxTemp > 200 && maxTemp < 255)
    {
        if (maxTemp + 20 > 255)
        {
            maxTemp = 255;
        }
        else
        {
            maxTemp += 20;
        }
    }
    else
    {
        maxTemp = 255;
    }
}

void TFT_Display::getTotalTimeFromProfile(ReflowProfile *profile)
{

    for (uint8_t i = 0; i < 5; i++)
    {
        totalTIme += profile->steps[i].duration;
    }


}

TFT_XY TFT_Display::getXYWithinGraphBounds(uint8_t temp, uint8_t time)
{

    TFT_XY xy;

    xy.x = graphXY.x + (graphWidth * (time / totalTIme));
    xy.y = graphXY.y - (graphHeight * ((temp - minTemp) / (maxTemp - minTemp)));

    return xy;
}

void TFT_Display::drawGraph()
{

    drawGraphAxis();
    drawGraphAxisLabels();
    drawGraphAxisTicks();
    drawGraphAxisTickLabels();
}

void TFT_Display::drawGraphAxis()
{

    tft.drawFastHLine(graphXY.x, graphXY.y, graphWidth, ST77XX_WHITE);

    tft.drawFastVLine(graphXY.x, graphXY.y - graphHeight, graphHeight, ST77XX_WHITE);
}

void TFT_Display::drawGraphAxisLabels()
{

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);

    tft.setCursor(5, 10);
    tft.println("Temp (C)");

    tft.setCursor(150, 28);
    tft.println("Time (s)");
}

void TFT_Display::drawGraphAxisTicks()
{

    // Temp start tick
    tft.drawFastHLine(graphXY.x - 5, graphXY.y, 5, ST77XX_WHITE);
    // Temp end tick
    tft.drawFastHLine(graphXY.x - 5, graphXY.y - graphHeight, 5, ST77XX_WHITE);

    // Draw three ticks on the temp axis at 1/4, 1/2 and 3/4 of the way along

    // 1/4
    tft.drawFastHLine(graphXY.x - 5, graphXY.y - (graphHeight / 4), tickMarkLength, ST77XX_WHITE);
    // 1/2
    tft.drawFastHLine(graphXY.x - 5, graphXY.y - (graphHeight / 2), tickMarkLength, ST77XX_WHITE);
    // 3/4
    tft.drawFastHLine(graphXY.x - 5, graphXY.y - (graphHeight * 3 / 4), tickMarkLength, ST77XX_WHITE);

    // Time start tick
    tft.drawFastVLine(graphXY.x, graphXY.y, 5, ST77XX_WHITE);
    // Time end tick
    tft.drawFastVLine(graphXY.x + graphWidth, graphXY.y, 5, ST77XX_WHITE);

    // Draw three ticks on the time axis at 1/4, 1/2 and 3/4 of the way along

    // 1/4
    tft.drawFastVLine(graphXY.x + (graphWidth / 4), graphXY.y, tickMarkLength, ST77XX_WHITE);
    // 1/2
    tft.drawFastVLine(graphXY.x + (graphWidth / 2), graphXY.y, tickMarkLength, ST77XX_WHITE);
    // 3/4
    tft.drawFastVLine(graphXY.x + (graphWidth * 3 / 4), graphXY.y, tickMarkLength, ST77XX_WHITE);
}

void TFT_Display::drawGraphAxisTickLabels()
{

    // Temperatures
    // Always starts at 20c
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    TFT_XY position = getRightAlignedTextXY("20", graphXY.x, graphXY.y);
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println("20");

    //   Always ends at maxTemp
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    char *maxTempCharPtr = numberToCharPtr(maxTemp);
    position = getRightAlignedTextXY(maxTempCharPtr, graphXY.x, graphXY.y - graphHeight);
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println(maxTemp);

    // Draw three tick labels on the temp axis at 1/4, 1/2 and 3/4 of the way along

    // 1/4
    uint8_t temp = (maxTemp - minTemp) / 4;
    char *tempCharPtr = numberToCharPtr(temp);
    position = getRightAlignedTextXY(tempCharPtr, graphXY.x, graphXY.y - (graphHeight / 4));
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println(temp);
    delete[] tempCharPtr;

    // 1/2
    temp = (maxTemp - minTemp) / 2;
    tempCharPtr = numberToCharPtr(temp);
    position = getRightAlignedTextXY(tempCharPtr, graphXY.x, graphXY.y - (graphHeight / 2));
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println(temp);
    delete[] tempCharPtr;

    // 3/4
    temp = (maxTemp - minTemp) * 3 / 4;
    tempCharPtr = numberToCharPtr(temp);
    position = getRightAlignedTextXY(tempCharPtr, graphXY.x, graphXY.y - (graphHeight * 3 / 4));
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println(temp);

    delete[] tempCharPtr;

    // Times

    // Always starts at 0
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    position = getCenterAlignedBottomTextXY("0", graphXY.x, graphXY.y);
    tft.setCursor(position.x, position.y+tickMarkLength+1);
    tft.println("0");

    // Always ends at totalTime
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    char *totalTimeCharPtr = numberToCharPtr(totalTIme);
    position = getCenterAlignedBottomTextXY(totalTimeCharPtr, graphXY.x + graphWidth, graphXY.y);
    tft.setCursor(position.x, position.y+tickMarkLength+1);
    tft.println(totalTIme);
    delete[] totalTimeCharPtr;

    // Draw three tick labels on the time axis at 1/4, 1/2 and 3/4 of the way along

    // 1/4
    uint8_t time = totalTIme / 4;
    char *timeCharPtr = numberToCharPtr(time);
    position = getCenterAlignedBottomTextXY(timeCharPtr, graphXY.x + (graphWidth / 4), graphXY.y);
    tft.setCursor(position.x, position.y+tickMarkLength+1);
    tft.println(time);
    delete[] timeCharPtr;

    // 1/2
    time = totalTIme / 2;
    timeCharPtr = numberToCharPtr(time);
    position = getCenterAlignedBottomTextXY(timeCharPtr, graphXY.x + (graphWidth / 2), graphXY.y);
    tft.setCursor(position.x, position.y+tickMarkLength+1);
    tft.println(time);
    delete[] timeCharPtr;

    // 3/4
    time = totalTIme * 3 / 4;
    timeCharPtr = numberToCharPtr(time);
    position = getCenterAlignedBottomTextXY(timeCharPtr, graphXY.x + (graphWidth * 3 / 4), graphXY.y);
    tft.setCursor(position.x, position.y+tickMarkLength+1);
    tft.println(time);
    delete[] timeCharPtr;
}

uint16_t TFT_Display::tempYonGraph(uint16_t temp)
{
    // Calculate the y position of the temperature on the graph based on the min and max temperatures and the min and max y of the graph

    uint8_t y = graphXY.y - (graphHeight * ((temp - minTemp) / (maxTemp - minTemp)));

    return y;
}

uint16_t TFT_Display::timeXonGraph(uint16_t time)
{
    // Calculate the x position of the time on the graph based on the min and max times and the min and max x of the graph

    uint8_t x = graphXY.x + (graphWidth * (time / totalTIme));

    return x;
}
