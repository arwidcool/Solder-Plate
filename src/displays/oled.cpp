#include "./oled.h"
#include <Arduino.h>
#include "../globals.h"
#include "./menustatemachine.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define MENUID_DEBUG 2
#define MENUITEM_THERMISTOR_START 150
#define MENUID_PICK_PROFILE 100
#define MENUITEM_PROFILE_START 100

unsigned long lastProcessedReflowState = 0;
OledDisplay::OledDisplay()
{

    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void OledDisplay::handleButtonStateChange(Pair<ButtonKind, StateChangeEvent<ButtonState>> change)
{
    ReflowProcessState state = reflowProcessState.get();
    if (state == USER_INPUT)
    {
        if (change.second.to == ButtonState::PRESSED)
        {
            if (change.first == ButtonKind::SELECT)
            {
                if (curMenu->identifier == MENUID_PICK_PROFILE)
                {
                    int profileIndex = curMenu->getCurItem().identifier - MENUITEM_PROFILE_START;
                    if (profileIndex >= 0 && profileIndex < nReflowProfiles)
                    {
                        chosenReflowProfile = reflowProfiles[profileIndex];
                        reflowProcessState.set(PREHEAT);
                        Serial.println("Chosen profile: " + String(chosenReflowProfile.name));
                    }
                    else
                    {
                        Serial.println("Invalid profile index: " + String(profileIndex));
                    }
                }
                else
                {

                    OledMenu *selectedMenu = curMenu->getNextMenu();
                    if (selectedMenu != NULL)
                    {
                        curMenu = selectedMenu;
                    }
                }
            }
            else if (change.first == ButtonKind::BACK)
            {
                OledMenu *selectedMenu = curMenu->parent;
                if (selectedMenu != NULL)
                {
                    curMenu = selectedMenu;
                }
            }
            else if (change.first == ButtonKind::UP)
            {
                curMenu->goPrevItem();
            }
            else if (change.first == ButtonKind::DOWN)
            {
                curMenu->goNextItem();
            }
        }
    }
    else if (state == DONE)
    {
        // GO back to iniital state
        if (change.second.to == ButtonState::PRESSED)
        {
            reflowProcessState.set(USER_INPUT);
        }
    }
}

void OledDisplay::handleDrawThermistorMenu(OledMenuItem menuItem)
{
    int thermistorIndex = menuItem.identifier - MENUITEM_THERMISTOR_START;
    // Serial.println("Thermistor index: " + String(thermistorIndex));
    if (thermistorIndex == 6)
    {
        // Showing all thermistors values in a row
        display.setTextSize(1, 2);
        for (int i = 0; i < 6; i++)
        {
            int thermistorTemp = thermistors[i].getTemperature();
            display.setCursor(i < 3 ? 0 : (SCREEN_WIDTH / 2 + 20), 20 * (i % 3));
            display.println(String(i + 1) + " " + String(thermistorTemp));
        }
        centerText(menuItem.title);
        displayIndicators();
    }
    else if (thermistorIndex == 7)
    {
        display.setTextSize(1, 2);
        for (int i = 0; i < 6; i++)
        {
            float thermR = thermistors[i].getResistance() / 1000;
            display.setCursor(i < 3 ? 0 : (SCREEN_WIDTH / 2 + 20), 20 * (i % 3));
            display.println(String(i + 1) + ":" + String(thermR));
        }
        centerText(menuItem.title);
        displayIndicators();
    }
    else
    {
        int thermistorTemp = thermistors[thermistorIndex].getTemperature();
        centerText((String(menuItem.title) + ": " + String(thermistorTemp)).c_str());
        displayIndicators();
    }
    display.display();
}

void OledDisplay::setup()
{
    curMenu = new OledMenu();
    curMenu->setElements(new OledMenuItem[3]{
                             OledMenuItem("Reflow\0"),
                             OledMenuItem("Temps\0"),
                             OledMenuItem("Debug\0"),
                         },
                         3);

    OledMenu *pickProfilesMenu = new OledMenu(MENUID_PICK_PROFILE);
    OledMenuItem *pickProfilesMenuItems = new OledMenuItem[nReflowProfiles];
    for (int i = 0; i < nReflowProfiles; i++)
    {
        pickProfilesMenuItems[i] = OledMenuItem(reflowProfiles[i].name, MENUITEM_PROFILE_START + i);
    }
    pickProfilesMenu->setElements(pickProfilesMenuItems, nReflowProfiles);

    OledMenu *debugMenu = new OledMenu(MENUID_DEBUG);
    OledMenu *tempsMenu = new OledMenu(3);
    tempsMenu->setElements(new OledMenuItem[8]{
                               OledMenuItem("C\0", MENUITEM_THERMISTOR_START + 6),
                               OledMenuItem("R(K)\0", MENUITEM_THERMISTOR_START + 7),
                               OledMenuItem("T1\0", MENUITEM_THERMISTOR_START + 0),
                               OledMenuItem("T2\0", MENUITEM_THERMISTOR_START + 1),
                               OledMenuItem("T3\0", MENUITEM_THERMISTOR_START + 2),
                               OledMenuItem("T4\0", MENUITEM_THERMISTOR_START + 3),
                               OledMenuItem("T5\0", MENUITEM_THERMISTOR_START + 4),
                               OledMenuItem("T6\0", MENUITEM_THERMISTOR_START + 5),
                           },
                           8);

    curMenu->setChildren(
        new OledMenu *[3]
        {
            pickProfilesMenu,
                tempsMenu,
                debugMenu,
        },
        3);
    curMenu->setChildrenMatrix(3, new uint8_t[3][2]{
                                      {0, 0},
                                      {1, 1},
                                      {2, 2},
                                  });

    // curItem = 0; // ROOT

    // Setup implementation
    bool initialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if (!initialized)
    {
        Serial.println("OLED Display failed to initialize");
    }
    else
    {
        Serial.println("OLED Display initialized");
    }
    display.setRotation(0);
    display.setTextColor(SSD1306_WHITE); // Defaults to white
}

void OledDisplay::loop()
{

    ReflowProcessState state = reflowProcessState.get();
    if (state == ReflowProcessState::USER_INPUT)
    {
        handleUserInputState();
    }
    else if (state >= ReflowProcessState::PREHEAT && state < ReflowProcessState::DONE)
    {
        handleReflowState();
    }
    else if (state == ReflowProcessState::DONE)
    {
        // Traverse back to root menu
        while (curMenu->parent != NULL)
        {
            curMenu = curMenu->parent;
        }
        display.clearDisplay();
        display.setRotation(0);
        display.setTextSize(2);
        drawPositionedText("DONE :)", DisplayTextAlignment::CENTER, DisplayTextAlignment::START);
        uint8_t curTemp = thermistor1.getTemperature();
        display.setTextSize(1, 2);
        drawPositionedText("Temperature", DisplayTextAlignment::START, DisplayTextAlignment::CENTER);
        drawPositionedText((String(curTemp) + " C").c_str(), DisplayTextAlignment::END, DisplayTextAlignment::CENTER);

        display.display();

        // This is not super efficient but it's fine for now
    }
    // Loop implementation
}

void OledDisplay::teardown()
{
    // delete display;
}

void OledDisplay::drawDebug()
{
    float sysVoltage = analogRef.calculateSystemVoltage();
    float inputVoltage = analogRef.calculateInputVoltage();
    int thermistor1Temp = thermistor1.getTemperature();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SysV: " + String(sysVoltage));
    display.setCursor(0, 20);
    display.println("In V:" + String(inputVoltage));
    display.setCursor(0, 40);
    display.println("C: " + String(thermistor1Temp));
    display.display();
}
void OledDisplay::displayIndicators()
{
    display.setTextSize(2);
    display.setRotation(1);
    display.setCursor(0, SCREEN_WIDTH / 2 - 5);
    display.print("<");
    display.setCursor(SCREEN_HEIGHT - 14, SCREEN_WIDTH / 2 - 5);
    display.print(">");
}
void OledDisplay::drawPositionedText(const char *txt, DisplayTextAlignment horizontal, DisplayTextAlignment vertical)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
    int cursorX = (horizontal == DisplayTextAlignment::CENTER ? display.width() / 2 - w / 2 : 0);
    if (horizontal == DisplayTextAlignment::END)
    {
        cursorX = display.width() - w;
    }
    int cursorY = (vertical == DisplayTextAlignment::CENTER ? display.height() / 2 - h / 2 : 0);
    if (vertical == DisplayTextAlignment::END)
    {
        cursorY = display.height() - h;
    }

    display.setCursor(cursorX, cursorY);

    display.println(txt);
}

void OledDisplay::handleUserInputState()
{
    display.clearDisplay();
    display.setRotation(0);
    display.setTextSize(2);

    OledMenuItem menuItem = curMenu->getCurItem();
    display.setTextSize(2);
    // THERMISTORS
    if (menuItem.identifier >= MENUITEM_THERMISTOR_START && menuItem.identifier < MENUITEM_THERMISTOR_START + 8)
    {
        handleDrawThermistorMenu(menuItem);
    }
    else if (curMenu->identifier == MENUID_DEBUG)
    {
        drawDebug();
    }
    else
    {
        // Default menu handling. Just display the title and the indicators to go back and forth
        if (strlen(menuItem.title) > 8)
        {
            display.setTextSize(1, 2);
        }
        else
        {
            display.setTextSize(2);
        }
        centerText(menuItem.title);
        displayIndicators();
        display.display();
    }
}
void OledDisplay::handleReflowState()
{
    display.clearDisplay();
    display.setRotation(0);
    display.setCursor(0, 0);
    display.setTextSize(2);
    ReflowProcessState state = reflowProcessState.get();
    // Title topleft
    drawPositionedText(STATE_STR(state), DisplayTextAlignment::CENTER, DisplayTextAlignment::START);

    display.setTextSize(1, 2);


    // Remaining time center left + bottom left
    uint32_t elapsedStep = chosenReflowProfile.getCurrentStepRelativeTime();
    drawPositionedText("Remaining", DisplayTextAlignment::START, DisplayTextAlignment::CENTER);
    drawPositionedText((String(chosenReflowProfile.reflowStep().duration - elapsedStep) + "s").c_str(), DisplayTextAlignment::START, DisplayTextAlignment::END);

    // Current temp center right + bottom right
    uint8_t curTemp = thermistor1.getTemperature();
    uint8_t targetTemp = pidControllerData.targetTemp;
    drawPositionedText(("Curr.: " + String(curTemp)).c_str(), DisplayTextAlignment::END, DisplayTextAlignment::CENTER);
    drawPositionedText(("Target: " + String(targetTemp)).c_str(), DisplayTextAlignment::END, DisplayTextAlignment::END);

    // display.println("In Voltage:"+String(systemVoltage));

    // SysV topright
    // #ifdef DEBUG
    display.setCursor(25, 15);
    display.setTextSize(1, 1);
    display.print(analogRef.calculateInputVoltage());
    // #endif

    display.display();
}
