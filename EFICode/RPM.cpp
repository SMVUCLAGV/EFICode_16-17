#include "RPM.h"
#include "Arduino.h"
#include "constants.h"

int getRPM (int timePassed, int rev) {
  return (60000000*rev)/(double)timePassed;
}

