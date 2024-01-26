#include "Buttons.h"

Button* __buttons[4] = { nullptr };


ButtonKind Buttons::getPressedButton()
{
  for (int i=0; i<4; i++) {
    if (__buttons[i]->getState() == ButtonState::PRESSED) {
      return __buttons[i]->getKind();
    }
  }

  return NONE;
}

Buttons::Buttons()
{
  initializeButtons();
}

void Buttons::initializeButtons()
{
  __buttons[0] = new Button(ButtonKind::UP, upButton);
  __buttons[1] = new Button(ButtonKind::DOWN, downButton);
  __buttons[2] = new Button(ButtonKind::BACK, backButton);
  __buttons[3] = new Button(ButtonKind::SELECT, selectButton);
}

ButtonKind Buttons::handleButtons()
{

  for (int i=0; i<4; i++) {
    __buttons[i]->loop();
  }
  return getPressedButton();
}
