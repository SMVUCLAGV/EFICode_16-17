#include "MAP.h"
#include "Arduino.h"
#include "constants.h"

double getOIN () {
  return analogRead(OIN_Pin)*voltageConversion;
}

bool isLean (int pin) {
  if (analogRead(OIN_Pin)*voltageConversion > 0.5) {
    return true;
  } else {
    return false;
  }
}

