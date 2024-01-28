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

    this->profile = profile;

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

    maxTemp = 0;

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

    totalTIme = 0;

    for (uint8_t i = 0; i < 5; i++)
    {
        totalTIme += profile->steps[i].duration;
    }

    Serial.println("Total time");
    Serial.println(String(totalTIme));
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

    drawGraphAxisLabels();
    drawGraphAxisTicks();
    drawGraphAxisTickLabels();
    drawGraphReflowStagesBackground();
    drawGraphAxis();
    drawReflowTargetTempLine();
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
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);

    TFT_XY position;

    // Always starts at 20c
    position = getRightAlignedTextXY("20", graphXY.x, graphXY.y);
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println("20");

    // Always ends at maxTemp
    char *maxTempCharPtr = numberToCharPtr(maxTemp);
    position = getRightAlignedTextXY(maxTempCharPtr, graphXY.x, graphXY.y - graphHeight);
    tft.setCursor(position.x - tickMarkLength, position.y);
    tft.println(maxTemp);
    delete[] maxTempCharPtr;

    // Draw three tick labels on the temp axis at 1/4, 1/2 and 3/4 of the way along
    for (int i = 1; i <= 3; i++)
    {
        uint8_t temp = (maxTemp - minTemp) * i / 4;
        char *tempCharPtr = numberToCharPtr(temp);
        position = getRightAlignedTextXY(tempCharPtr, graphXY.x, graphXY.y - (graphHeight * i / 4));
        tft.setCursor(position.x - tickMarkLength, position.y);
        tft.println(temp);
        delete[] tempCharPtr;
    }

    // Times
    // Always starts at 0
    position = getCenterAlignedBottomTextXY("0", graphXY.x, graphXY.y);
    tft.setCursor(position.x, position.y + tickMarkLength + 1);
    tft.println("0");

    // Always ends at totalTime
    char *totalTimeCharPtr = numberToCharPtr(totalTIme);
    position = getCenterAlignedBottomTextXY(totalTimeCharPtr, graphXY.x + graphWidth, graphXY.y);
    tft.setCursor(position.x, position.y + tickMarkLength + 1);
    tft.println(totalTIme);
    delete[] totalTimeCharPtr;

    // Draw three tick labels on the time axis at 1/4, 1/2 and 3/4 of the way along
    for (int i = 1; i <= 3; i++)
    {
        uint8_t time = totalTIme * i / 4;
        char *timeCharPtr = numberToCharPtr(time);
        position = getCenterAlignedBottomTextXY(timeCharPtr, graphXY.x + (graphWidth * i / 4), graphXY.y);
        tft.setCursor(position.x, position.y + tickMarkLength + 1);
        tft.println(time);
        delete[] timeCharPtr;
    }
}

void TFT_Display::drawGraphReflowStagesBackground()
{
    uint16_t x = graphXY.x + 1;
    uint16_t previopusWidth;
    // Draw the background for the reflow stages
    for (int i = 0; i < 5; i++)
    {

        Serial.println("Time x");
        Serial.println(String(x));
        uint16_t y = graphXY.y - graphHeight;

        uint16_t height = graphHeight;

        // Get the width by getting the duration of each step in relation to the total time

        float duration = profile->steps[i].duration;

        uint16_t width = (duration / totalTIme) * graphWidth;

        switch (i)
        {
        case 0:
            tft.drawRect(x, y, width, height, preheat_COLOR);
            previopusWidth = width;
            break;

        case 1:
            x += previopusWidth;
            tft.drawRect(x, y, width, height, soak_COLOR);
            previopusWidth = width;
            break;

        case 2:
            x += previopusWidth;
            tft.drawRect(x, y, width, height, reflow_COLOR);
            previopusWidth = width;
            break;

        case 3:
            x += previopusWidth;
            tft.drawRect(x, y, width, height, cool_COLOR);
            previopusWidth = width;
            break;

        default:

            break;
        }
    }
}

void TFT_Display::drawReflowTargetTempLine()
{

    float startTemp = 20;
    int startTime = 0;
    uint16_t startX = graphXY.x;
    uint16_t startY = graphXY.y;
    int endTime;

    for (int i = 0; i < 4; i++)
    {
        // Get the end temp and time of the current step
        int endTemp = profile->steps[i].targetTempAtEnd;
        int duration = profile->steps[i].duration;

        endTime = duration + startTime;
        uint16_t endX = timeXonGraph(endTime);
        uint16_t endY = tempYonGraph(endTemp);

        Serial.print("State:");
        Serial.println(String(STATE_STR(profile->steps[i].state)));
        Serial.print("Start x:");
        Serial.println(String(startX));
        Serial.print("Start y:");
        Serial.println(String(startY));
        Serial.print("End x:");
        Serial.println(String(endX));
        Serial.print("End y:");
        Serial.println(String(endY));
        Serial.print("Start temp:");
        Serial.println(String(startTemp));
        Serial.print("End temp:");
        Serial.println(String(endTemp));
        Serial.print("Start time:");
        Serial.println(String(startTime));
        Serial.print("End time:");
        Serial.println(String(endTime));
        Serial.println(" ");


        if(profile->steps[i].flagged) {
          //  tft.drawCircle(startX, startY, 5, ST77XX_RED);
            startTemp = profile->steps[i-2].targetTempAtEnd;
            
        }

        // Draw the line pixel by pixel
        switch (profile->steps[i].easeFunction)
        {
        case LINEAR:
            tft.drawLine(startX, startY, endX, endY, ST77XX_WHITE);

            break;

        case EASE_IN_OUT:

            for (int i = 0; i <= 100; i++)
            {
                // Convert i to a percentage
                float percentage = i / 100.0;

                // calculate the temp at the current percentage of the line

                float temp = startTemp + (endTemp - startTemp) * -(cos(percentage * PI) - 1) / (double)2;

                Serial.print("Temp:");
                Serial.println(temp);

                Serial.print("Percentage:");
                Serial.println(percentage);

                float time = startTime + (duration * percentage);

                // Calculate the x and y position of the temp on the graph

                uint16_t y = tempYonGraph(temp);

                uint16_t x = timeXonGraph(time);

                tft.drawPixel(x, y, ST77XX_WHITE);
            }

            break;
        case EASE_IN:

            for (int i = 0; i <= 100; i++)
            {
                // Convert i to a percentage
                float percentage = i / 100.0;

                // calculate the temp at the current percentage of the line

                float temp = startTemp + (endTemp - startTemp) * (1 - cos(percentage * PI / (double)2));

                Serial.print("Temp:");
                Serial.println(temp);

                Serial.print("Percentage:");
                Serial.println(percentage);

                // Calculate the x and y position of the temp on the graph

                uint16_t y = tempYonGraph(temp);

                uint16_t x = timeXonGraph(startTime + (duration * percentage));

                tft.drawPixel(x, y, ST77XX_WHITE);
            }
            break;
        case EASE_OUT:
            for (int i = 0; i <= 100; i++)
            {
                // Convert i to a percentage
                float percentage = i / 100.0;

                // calculate the temp at the current percentage of the line

        

                float temp = startTemp + (endTemp - startTemp) * (sin(percentage * PI / (double)2));

                Serial.print("Temp:");
                Serial.println(temp);

                Serial.print("Percentage:");
                Serial.println(percentage);

                // Calculate the x and y position of the temp on the graph

                uint16_t y = tempYonGraph(temp);

                uint16_t x = timeXonGraph(startTime + (duration * percentage));

                tft.drawPixel(x, y, ST77XX_WHITE);
            }
            break;

        case HALF_SINE:

            for (int i = 0; i <= 100; i++)
            {
                // Convert i to a percentage
                float percentage = i / 100.0;

                // calculate the temp at the current percentage of the line

                float temp = startTemp + (endTemp - startTemp) * (sin(percentage * PI));

                Serial.print("Temp:");
                Serial.println(temp);

                Serial.print("Percentage:");
                Serial.println(percentage);

                // Calculate the x and y position of the temp on the graph

                uint16_t y = tempYonGraph(temp);

                uint16_t x = timeXonGraph(startTime + (duration * percentage));

                tft.drawPixel(x, y, ST77XX_WHITE);
            }

            break;
        default:

            // tft.drawLine(startX, startY, endX, endY, ST77XX_WHITE);
            break;
        }

        // tft.drawLine(startX, startY, endX, endY, ST77XX_WHITE);

        // Set the start temp and time to the end temp and time of the current step
        startTemp = endTemp;
        startTime = endTime;

        // Get the start and end x and y positions of the line
        startX = endX;
        startY = endY;
    }
}

uint16_t TFT_Display::tempYonGraph(float temp)
{

    // Calculate the y position of the temperature on the graph based on the min and max temperatures and the min and max y of the graph the higher the temperature the lower the y values goes

    float y = graphXY.y - (graphHeight * ((temp - minTemp) / (maxTemp - minTemp)));

    return y;
}

uint16_t TFT_Display::timeXonGraph(float time)
{
    // Calculate the x position of the time on the graph based on the min and max times and the min and max x of the graph

    float x = graphXY.x + (graphWidth * (time / totalTIme));

    return x;
}
