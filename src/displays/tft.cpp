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

    prevousTempXY.x = graphXY.x;
    prevousTempXY.y = graphXY.y;

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    TFT_XY xy = getCenteredTextXY(profile->name);
    tft.setCursor(xy.x, xy.y);
    tft.println(profile->name);
    getMaxTempFromProfile(profile);

    this->profile = profile;

    drawTimer.reset();

    drawTimer.setResolution(StopWatch::MILLIS);
    drawTimer.start();

    drawGraph();
}

void TFT_Display::drawRealTemp(double *temp, float percentage)
{

    uint32_t timeSinceLastDraw = drawTimer.elapsed();

    // Serial.print("Time since last draw:");
    // Serial.println(timeSinceLastDraw);

    if (timeSinceLastDraw >= 930)
    {

        float temperature = static_cast<float>(*temp);

        // Serial.print("Time:");
        // Serial.print(percentage);
        // Serial.print(" Temp:");
        // Serial.println(temperature);

        uint16_t eplased = drawTimer.elapsed();

        // Get the xy and y pixel of the temp

        uint16_t x = percentageToX(percentage);
        uint16_t y = tempYonGraph(temperature);
        // Draw the pixel

        //tft.drawPixel(x, y, ST77XX_RED);
        
        tft.drawLine(prevousTempXY.x, prevousTempXY.y, x, y, ST77XX_RED);

        prevousTempXY.x = x;
        prevousTempXY.y = y;
        drawTimer.reset();
        drawTimer.start();
    }
    
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
        if (profile->steps[i].targetTemp > maxTemp)
        {
            maxTemp = profile->steps[i].targetTemp;
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
    uint16_t totalTime = profile->endTimes[4];
    char *totalTimeCharPtr = numberToCharPtr(totalTime);
    position = getCenterAlignedBottomTextXY(totalTimeCharPtr, graphXY.x + graphWidth, graphXY.y);
    tft.setCursor(position.x, position.y + tickMarkLength + 1);
    tft.println(totalTime);
    delete[] totalTimeCharPtr;

    // Draw three tick labels on the time axis at 1/4, 1/2 and 3/4 of the way along
    for (int i = 1; i <= 3; i++)
    {
        uint8_t time = totalTime * i / 4;
        char *timeCharPtr = numberToCharPtr(time);
        position = getCenterAlignedBottomTextXY(timeCharPtr, graphXY.x + (graphWidth * i / 4), graphXY.y);
        tft.setCursor(position.x, position.y + tickMarkLength + 1);
        tft.println(time);
        delete[] timeCharPtr;
    }
}

void TFT_Display::drawGraphReflowStagesBackground()
{
    uint16_t colors[4] = {preheat_COLOR, soak_COLOR, reflow_COLOR, cool_COLOR};
    // Draw the background for the reflow stages
    uint16_t x= graphXY.x + 1;
    uint16_t y= graphXY.y;
    uint16_t totalDuration = profile->endTimes[4];
    for (int i=0; i<4; i++) {
        uint16_t y = graphXY.y - graphHeight;
        uint16_t height = graphHeight;
        float duration = profile->endTimes[i];
        float percAtEndStep = duration / totalDuration;
        float newX = percentageToX(percAtEndStep);
        Serial.println(String(i) + " - duration: "+ String(duration) + " |Perc: "+ String(percAtEndStep)+ " - x:" + String(x) + " y:" + String(y) + " newX:" + String(newX) + " height:" + String(height) + " color:" + String(colors[i]));
        tft.drawRect(x, y, newX-x, graphHeight, colors[i]);
        x = newX;
        
    }
}


void TFT_Display::drawReflowTargetTempLine() {
    uint16_t y = tempYonGraph(chosenReflowProfile.startTemps[0]);
    uint16_t x = percentageToX(0);
    for (float i=0.00; i<=1; i+=0.01) {
        float temp = profile->getTargetTempFromPercentage(i);
        uint16_t y2 = tempYonGraph(temp);
        uint16_t x2 = percentageToX(i);
        // Serial.println(String(i) + " - temp: "+ String(temp) + " - x:" + String(x) + " y:" + String(y) + " x2:" + String(x2) + " y2:" + String(y2));
        tft.drawLine(x, y, x2, y2, ST77XX_WHITE);
        x = x2;
        y = y2;
    }
}

uint16_t TFT_Display::tempYonGraph(float temp)
{

    // Calculate the y position of the temperature on the graph based on the min and max temperatures and the min and max y of the graph the higher the temperature the lower the y values goes

    float y = graphXY.y - (graphHeight * ((temp - minTemp) / (maxTemp - minTemp)));

    return y;
}

uint16_t TFT_Display::tempYonGraph(float *temp)
{

    float y = graphXY.y - (graphHeight * ((*temp - minTemp) / (maxTemp - minTemp)));
    return y;
}

uint16_t TFT_Display::percentageToX(float percentage)
{
    // Calculate the x position of the time on the graph based on the min and max times and the min and max x of the graph

    return graphXY.x + (graphWidth * percentage);
}
