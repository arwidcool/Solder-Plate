#include "Buttons.h"

extern bool yellowLedON;

 Button *__buttons[4];

ButtonKind Buttons::getPressedButton()
{
  for (int i = 0; i < 4; i++)
  {
    if (__buttons[i]->getState() == ButtonState::PRESSED)
    {
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

Button** Buttons::getAllButtons()
{

  return __buttons;
}

void Buttons::handleButtonLEDs()
{

  for (int i = 0; i < 4; i++)
  {
    if (__buttons[i]->getState() == ButtonState::PRESSED)
    {

      switch (__buttons[i]->getKind())
      {
      case ButtonKind::UP:
        Serial.println("UP");
        if (!yellowLedON)
        {
          digitalWrite(yellowLED, HIGH);
          yellowLedON = true;
        }
        break;
      case ButtonKind::DOWN:
        Serial.println("DOWN");
        if (!yellowLedON)
        {
          digitalWrite(yellowLED, HIGH);
          yellowLedON = true;
        }
        break;
      case ButtonKind::BACK:

        Serial.println("BACK");

        digitalWrite(redLED, HIGH);

        break;

      case ButtonKind::SELECT:

        Serial.println("SELECT");
        analogWrite(greenLED, 20);

        break;

      default:

        break;
      }
    }
    else if (__buttons[i]->getState() == ButtonState::RELEASED)
    {
      switch (__buttons[i]->getKind())
      {
      case ButtonKind::BACK:

        digitalWrite(redLED, LOW);

        break;

      case ButtonKind::SELECT:

        digitalWrite(greenLED, LOW);
        break;

      case ButtonKind::UP:

        if (yellowLedON)
        {
          digitalWrite(yellowLED, LOW);
          yellowLedON = false;
        }
        break;
      case ButtonKind::DOWN:
        if (yellowLedON)
        {
          digitalWrite(yellowLED, LOW);
          yellowLedON = false;
        }

        break;
      default:
        break;
      }
    }
  }
}

ButtonKind Buttons::handleButtons()
{

  for (int i = 0; i < 4; i++)
  {
    __buttons[i]->loop();
  }
  
  return getPressedButton();
}
