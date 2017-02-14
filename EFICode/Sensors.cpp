#include "Controller.h"

#include "Arduino.h"
#include "Constants.h"

const double voltageConversion = .0049;

int Controller::getRPM (long int timePassed, int rev) {
  return (60000000.0 * rev) / (timePassed);
}

//TPS Measurement
//oldTPSConversion = 0.401738191;
//oldTPSOffset = -.84
//TPSConversion = oldTPSConversion * voltageConversion
//TPSOffset = oldTPSOffset * oldtpsConversion
const double TPSConversion = .0019685;
const double TPSOffset = -.33746;

double Controller::getTPS() {
  //gets throttle position based off of the percentage of throttle area open
  //TODO: Create sin lookup table
  return sin(TPSConversion * analogRead(TPS_Pin) + TPSOffset);
}

//Temperature Measurement
//oldTempSlope = -19.451
//oldTempIntercept = 89.135;
//tempSlope = oldTempSlope * voltageConversion
//tempIntercept = oldTempIntercept + 273
const double tempSlope = -.0953;
const double tempIntercept = 362.135;

double Controller::getTemp(int pin) {
  //Gets temperature ready by specified sensor by using calibration curve
  return tempSlope * analogRead(pin) + tempIntercept;
}

//MAP Measurement
//scalingFactor = 1000
//oldMAPConversion = 18.8636364
//oldMAPOffset = 20
const double MAPConversion = 92.432; // = oldMAPConversion * voltageConversion * scalingFactor
const double MAPOffset = 20000; // = oldMAPOffset * scalingFactor

double Controller::getMAP() {
  //Calculates MAP, outputs in Pa
  return MAPConversion * analogRead(MAP_Pin) + MAPOffset;
}

double Controller::getOIN () {
  //Gets Reading from O2 Sensor
  //TODO: figure out units
  return voltageConversion * analogRead(OIN_Pin);
}

bool Controller::isLean (int pin) {
  //TODO: see if this is useful
  return (voltageConversion * analogRead(pin)) > 0.5;
}
