#include "temperature.h"
#include "Arduino.h"
#include "constants.h"

double getTemp(int pin) {
  return (analogRead(pin)*voltageConversion*(slope)+intercept) + 273;
  //return analogRead(pin);
}
