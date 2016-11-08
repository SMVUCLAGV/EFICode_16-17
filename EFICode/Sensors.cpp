#include "Sensors.h"
#include "Arduino.h"
#include "constants.h"

int getRPM (int timePassed, int rev) {
  return (60000000*rev)/(double)timePassed;
}

double getTPS() {
  //gets throttle position based off of the percentage of throttle area open
  double angle = (throttlePositionConversion * (analogRead(TPS_Pin)*voltageConversion -.84));
  return sin(angle);
}

double getTemp(int pin) {
  return (analogRead(pin)*voltageConversion*(slope)+intercept) + 273;
  //return analogRead(pin);
}

double getMAP() {
  // Output in Pa
  return (18.8636364*analogRead(MAP_Pin)*voltageConversion + 20)*1000;
}

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
