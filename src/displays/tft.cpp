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
    tft.println("Temp");

    TFT_XY position = getCenteredTextXY("Time");
    tft.setCursor(position.x, position.y +228);
    tft.println("Time");


}

void TFT_Display::drawGraphAxisTicks()
{
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    TFT_XY position = getRightAlignedTextXY("20", graphXY.x, graphXY.y);
    tft.setCursor(position.x, position.y);
    tft.println("20");
}
