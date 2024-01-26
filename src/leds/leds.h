#ifndef __leds_h__
#define __leds_h__
#include "../buttons/base.h"
#include "../common.h"

//If you didnt solder the LEDS in order, change the order here
#define yellowLED 18
#define greenLED 19
#define redLED 20

class LEDS
{
public:
  LEDS();
  void setup();
  void handleButtonStateChange(Pair<ButtonKind, StateChangeEvent<ButtonState>> change);
  
};
#endif