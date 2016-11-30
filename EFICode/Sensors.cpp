#include "Sensors.h"
#include "Arduino.h"
#include "constants.h"

int getRPM (int timePassed, int rev) {
  return (60000000 * rev) / ((double) timePassed);
}

double getTPS() {
  //gets throttle position based off of the percentage of throttle area open
  double angle = TPSConversion * analogRead(TPS_Pin) + TPSOffset;
  return sin(angle);
}

double getTemp(int pin) {
  //Gets temperature ready by specified sensor by using calibration curve
  return tempSlope * analogRead(pin) + tempIntercept;
}

double getMAP() {
  // Calculates MAP, outputs in Pa
  return MAPConversion * analogRead(MAP_Pin) + MAPOffset;
}

double getOIN () {
  return voltageConversion * analogRead(OIN_Pin);
}

bool isLean (int pin) {
  double val = voltageConversion * analogRead(pin);
  return (val > 0.5);
}
