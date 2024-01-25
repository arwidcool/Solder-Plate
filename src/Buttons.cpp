#include "Buttons.h"

volatile bool upButtonPressed = false;
volatile bool downButtonPressed = false;
volatile bool backButtonPressed = false;
volatile bool selectButtonPressed = false;

ButtonState upButtonState = IDLE;
ButtonState downButtonState = IDLE;
ButtonState backButtonState = IDLE;
ButtonState selectButtonState = IDLE;


Button Buttons::getPressedButton()
{
  if (upButtonPressed)
  {
    upButtonPressed = false;

    return UP;
  }
  else if (downButtonPressed)
  {
    downButtonPressed = false;

    return DOWN;
  }
  else if (backButtonPressed)
  {
    backButtonPressed = false;

    return BACK;
  }
  else if (selectButtonPressed)
  {
    selectButtonPressed = false;

    return SELECT;
  }

  return NONE;
}

Buttons::Buttons()
{
  initializeButtons();
}

Buttons::~Buttons()
{
}

void Buttons::initializeButtons()
{

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(upButton), upButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(downButton), downButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(backButton), backButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(selectButton), selectButtonISR, FALLING);
}

Button Buttons::handleButtons()
{

  Button pressedButton = getPressedButton();

  if (pressedButton != NONE)
  {
    Serial.print("Button pressed: ");
    switch (pressedButton)
    {
    case UP:
      Serial.println("UP");
      digitalWrite(yellowLED, HIGH);
      delay(100);
      digitalWrite(yellowLED, LOW);
      break;
    case DOWN:
      Serial.println("DOWN");
      digitalWrite(yellowLED, HIGH);
      delay(100);
      digitalWrite(yellowLED, LOW);
      break;
    case BACK:
      Serial.println("BACK");
      digitalWrite(redLED, HIGH);
      delay(100);
      digitalWrite(redLED, LOW);
      break;
    case SELECT:
      Serial.println("SELECT");
      analogWrite(greenLED, 20);
      delay(100);
      analogWrite(greenLED, 0);

      break;
    default:
      break;
    }
  }

  return pressedButton;
}

void Buttons::upButtonISR()
{

  upButtonPressed = true;
}

void Buttons::downButtonISR()
{
  downButtonPressed = true;
}

void Buttons::backButtonISR()
{
  backButtonPressed = true;
}

void Buttons::selectButtonISR()
{
  selectButtonPressed = true;
}
