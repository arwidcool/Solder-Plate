#ifndef __OLEDSTATEMACHINE_H_
#define __OLEDSTATEMACHINE_H_
#include "../buttons/base.h"
#include "../common.h"
#include <Arduino.h>


#ifdef DEBUG_OLED
#define debugLine(x)  Serial.println(x)
#define debugC(x) Serial.print(x)
#else
#define debugLine(x)
#define debugC(x)
#endif


class OledMenuItem
{
public:
    OledMenuItem(){};
    OledMenuItem(char *title) : title(title), identifier(0){};
    OledMenuItem(char *title, uint8_t identifier) : title(title), identifier(identifier){};

    char const *title;
    // Identifier c
    uint8_t identifier;
};

class OledMenu
{
public:
    OledMenu(){};
    OledMenu(uint8_t identifier) : identifier(identifier), elementsLength(0), childrenLength(0){};
    ~OledMenu()
    {
        delete elements;
        delete children;
        delete childrenMatrix;
    };
    uint8_t identifier;

    void setChildren(OledMenu **children, int length)
    {
        this->children = children;
        this->childrenLength = length;
        for (int i = 0; i < length; i++)
        {
            children[i]->parent = this;
        }
    }

    void setElements(OledMenuItem *elements, int length)
    {
        this->elements = new OledMenuItem[length];

        for (int i = 0; i < length; i++)
        {
            this->elements[i].title = malloc(strlen(elements[i].title) + 1);
            memcpy(this->elements[i].title, elements[i].title, strlen(elements[i].title) + 1);
            this->elements[i].identifier = elements[i].identifier;

            debugLine(String(this->elements[i].title) + " " + String(elements[i].title));
        }
        this->elementsLength = length;
    }

    void setChildrenMatrix(int length, uint8_t (*matrix)[2])
    {
        this->childrenMatrix = matrix;
        this->childrenMatrixLength = length;
    }

    // To be invoked when the user presses the SELECT btn
    // If null is returned then the menu should not be changed
    OledMenu *getNextMenu()
    {
        for (int i = 0; i < childrenMatrixLength; i++)
        {
            if (childrenMatrix[i][0] == curItem)
            {
                debugLine(String(childrenMatrix[i][0]) + " " + String(childrenMatrix[i][1]));
                return children[childrenMatrix[i][1]];
            }
        }
        return nullptr;
    }

    // To be invoked when the user presses the UP btn
    OledMenuItem goNextItem()
    {
        curItem++;
        curItem = curItem % elementsLength;
        debugLine(String(elements[curItem].title) + " - " + String(curItem));
        return elements[curItem];
    }

    // To be invoked when the user presses the DOWN btn
    OledMenuItem goPrevItem()
    {
        curItem--;
        if (curItem < 0)
        {
            curItem = elementsLength - 1;
        }
        debugLine(String(elements[curItem].title) + " - " + String(curItem));

        return elements[curItem];
    }

    OledMenuItem itemAt(uint8_t index)
    {
        return elements[index];
    }

    OledMenuItem getCurItem()
    {
        return elements[curItem];
    }

    OledMenu *parent = nullptr;

protected:
    int curItem = 0;
    OledMenuItem *elements;
    int elementsLength;
    OledMenu **children;
    int childrenLength;
    /**
     * Matrix of pairs of (element index, children index)
     * ex if element at index 0 should go to children at index 1 when user hits SELECT
     * then the matrix would be:
     * { ..., {0, 1}, ...}
     */
    uint8_t (*childrenMatrix)[2]; // First is element index, second is children index,
    int childrenMatrixLength;
};

#define SETMATRIX(menuItem, CODE)                                       \
    {                                                                   \
        Pair<ButtonKind, int> matrix[] = {CODE};                        \
        menuItem.setMatrix(sizeof(matrix) / sizeof(matrix[0]), matrix); \
    }
#define UPBUTTON(index) Pair<ButtonKind, int>(ButtonKind::UP, index),
#define DOWNBUTTON(index) Pair<ButtonKind, int>(ButtonKind::DOWN, index),
#define SELECTBUTTON(index) Pair<ButtonKind, int>(ButtonKind::SELECT, index),
#define BACKBUTTON(index) Pair<ButtonKind, int>(ButtonKind::BACK, index),
// #define SETOLED_HANDLER(handler, CODE) { handler.buttonPressed = [](ButtonKind kind) -> OledInterfaceHandler* { CODE }; }
// #define BTNGOTOHANDLER(_kind, gotoHandler) if (kind == _kind) { return gotoHandler; }
// #define HANDLER_ONUP(gotoHandler) BTNGOTOHANDLER(ButtonKind::UP, gotoHandler)
// #define HANDLER_ONDOWN(gotoHandler) BTNGOTOHANDLER(ButtonKind::DOWN, gotoHandler)
// #define HANDLER_ONBACK(gotoHandler) BTNGOTOHANDLER(ButtonKind::BACK, gotoHandler)
// #define HANDLER_ONSELECT(gotoHandler) BTNGOTOHANDLER(ButtonKind::SELECT, gotoHandler)
// #define HANDLER_NULL return nullptr;

#endif
