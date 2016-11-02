#include "TPS.h"
#include "Arduino.h"
#include <math.h>
#include "constants.h"

double getTPS() //gets throttle position based off of the percentage of throttle area open
{
  double angle = (throttlePositionConversion * (analogRead(TPS_Pin)*voltageConversion -.84));
  return sin(angle);
}
