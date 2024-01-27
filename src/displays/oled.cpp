#include "./oled.h"
#include <Arduino.h>
#include "../globals.h"
#include "./menustatemachine.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define MENUID_PLATERES 2
#define MENUITEM_THERMISTOR_START 150

unsigned long lastProcessedReflowState = 0;
OledDisplay::OledDisplay()
{

    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void OledDisplay::handleButtonStateChange(Pair<ButtonKind, StateChangeEvent<ButtonState>> change)
{
    if (change.second.to == ButtonState::PRESSED)
    {
        if (change.first == ButtonKind::SELECT)
        {
            OledMenu *selectedMenu = curMenu->getNextMenu();
            if (selectedMenu != NULL)
            {
                curMenu = selectedMenu;
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
            curMenu->goNextItem();
        }
        else if (change.first == ButtonKind::DOWN)
        {
            curMenu->goPrevItem();
        }
    }
}
void OledDisplay::setup()
{
    curMenu = new OledMenu();
    curMenu->setElements(new OledMenuItem[3]{
                             OledMenuItem("Reflow\0"),
                             OledMenuItem("PlateR\0"),
                             OledMenuItem("Temps\0"),
                         },
                         3);

    OledMenu *pickProfilesMenu = new OledMenu(1);
    OledMenuItem *pickProfilesMenuItems = new OledMenuItem[nReflowProfiles];
    for (int i = 0; i < nReflowProfiles; i++)
    {
        pickProfilesMenuItems[i] = OledMenuItem(reflowProfiles[i].name, 100 + 1);
    }
    pickProfilesMenu->setElements(pickProfilesMenuItems, nReflowProfiles);

    OledMenu *plateRMenu = new OledMenu(MENUID_PLATERES);
    OledMenu *tempsMenu = new OledMenu(3);
    tempsMenu->setElements(new OledMenuItem[7]{
                               OledMenuItem("ALL\0", MENUITEM_THERMISTOR_START + 6),
                               OledMenuItem("T1\0", MENUITEM_THERMISTOR_START + 0),
                               OledMenuItem("T2\0", MENUITEM_THERMISTOR_START + 1),
                               OledMenuItem("T3\0", MENUITEM_THERMISTOR_START + 2),
                               OledMenuItem("T4\0", MENUITEM_THERMISTOR_START + 3),
                               OledMenuItem("T5\0", MENUITEM_THERMISTOR_START + 4),
                               OledMenuItem("T6\0", MENUITEM_THERMISTOR_START + 5),
                           },
                           6);

    curMenu->setChildren(
        new OledMenu *[3]
        {
            pickProfilesMenu,
                plateRMenu,
                tempsMenu,
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
    if (state == USER_INPUT)
    {
        display.clearDisplay();
        display.setRotation(0);
        display.setTextSize(2);

        OledMenuItem menuItem = curMenu->getCurItem();
        display.setTextSize(2);
        if (menuItem.identifier >= MENUITEM_THERMISTOR_START && menuItem.identifier < MENUITEM_THERMISTOR_START + 7)
        {
            int thermistorIndex = menuItem.identifier - MENUITEM_THERMISTOR_START;
            if (thermistorIndex == 6) {
                // Showing all
            } else {
                int thermistorTemp = thermistors[thermistorIndex].getTemperature();
                centerText((String(menuItem.title) + ": " + String(thermistorTemp)).c_str());
                displayIndicators();
            }
            
        }   
        else if (curMenu->identifier == MENUID_PLATERES)
        {
            // TODO: Logic for plate resistance
        }
        else
        {

            // draw menu item with selectors.

            centerText(menuItem.title);
            displayIndicators();
        }

        display.display();
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
    display.println("In V: " + String(inputVoltage));
    display.setCursor(0, 40);
    display.println("C°: " + String(thermistor1Temp));
    display.display();
}
void OledDisplay::displayIndicators()
{
    display.setRotation(1);
    display.setCursor(0, SCREEN_WIDTH / 2 - 5);
    display.print("<");
    display.setCursor(SCREEN_HEIGHT - 14, SCREEN_WIDTH / 2 - 5);
    display.print(">");
}
void OledDisplay::centerText(const char *txt)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(display.width() / 2 - w / 2, display.height() / 2 - h / 2);

    display.println(txt);
}

void OledDisplay::handleUserInputState()
{
    display.clearDisplay();
    display.setCursor(0, SCREEN_HEIGHT / 2 + 10);
    display.setTextSize(2);

    display.display();
}
void OledDisplay::handleReflowState()
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("REFLOW");
    display.display();
}
