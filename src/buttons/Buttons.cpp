#include "Buttons.h"

Button *__buttons[4];

void Buttons::setup()
{
  __buttons[ButtonKind::UP] = new Button(ButtonKind::UP, upButton);
  __buttons[ButtonKind::DOWN] = new Button(ButtonKind::DOWN, downButton);
  __buttons[ButtonKind::BACK] = new Button(ButtonKind::BACK, backButton);
  __buttons[ButtonKind::SELECT] = new Button(ButtonKind::SELECT, selectButton);
}
/**
 * Handle buttons state changes and return first button state change that occurred.
 */
Pair<ButtonKind, StateChangeEvent<ButtonState>>* Buttons::handleButtons()
{
  for (int i = 0; i < 4; i++)
  {
    // If a state change occurred, print it out and return the button that changed
    if (__buttons[i]->loop())
    {
      StateChangeEvent<ButtonState> *change = __buttons[i]->lastChange();

      Serial.println(String("ButtonStateChange: kind=") + BTNKIND_STR(__buttons[i]->kind) + ", " + BTNSTATECHANGE_STR((*change)));
      return new Pair<ButtonKind, StateChangeEvent<ButtonState>>(__buttons[i]->kind, *change);
    }
  }
  return NULL;
}
