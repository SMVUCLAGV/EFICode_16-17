#include "MAP.h"
#include "Arduino.h"
#include "constants.h"

double getMAP() { // Output in Pa
  return (18.8636364*analogRead(MAP_Pin)*voltageConversion + 20)*1000;
}
